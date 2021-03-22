#ifndef SEMPR_COMPONENTQUERY_HPP_
#define SEMPR_COMPONENTQUERY_HPP_

#include <vector>
#include <type_traits>
#include <utility>
#include <tuple>
#include <array>

#include "nodes/SopranoModule.hpp" // for SPARQL-Queries etc
#include "nodes/ComponentQueryNode.hpp"
#include "Utility.hpp"


namespace sempr {


    /**
        A container to attach more information to the result of a component
        query.
    */
    template <class T>
    struct ComponentQueryResult {
        Entity::Ptr         entity;
        std::shared_ptr<T>  component;
        std::string         tag;

        bool isInferred() const
        {
            auto allComponents = entity->getComponents<Component>();
            auto it = std::find(
                allComponents.begin(), allComponents.end(),
                component);

            return it == allComponents.end();
        }

        operator bool () const
        {
            return (entity && component);
        }

    };

    /**
        ComponentQuery objects are extended SPARQL queries: In the first step,
        a simple SPARQL query is executed. Afterwards, specific datatypes are
        extracted by searching them in ECWMEs, where the given variable from
        the SPARQL query is mapped to the ID of the entity.

        E.g.:
        ComponentQuery<>("SELECT * WHERE { ?e rdf:type agriculture:Field . }")
            .with<GeosGeometryI>("e")
            .execute();

        Creates and executes a query for (semantic) fields and augments the
        results from the sparql query by adding the GeosGeometryI-components
        associated with the entity with the id stored in "?e".
    */
    template <class... Ts> class ComponentQuery;

    /**
        The base class of all component queries. Basically just a SPARQL query.
    */
    template <>
    class ComponentQuery<> {
    public:
        typedef std::tuple<SPARQLQuery::Bindings> ResultType;

        virtual ~ComponentQuery() = default;

        /**
            Creates basically a sparql query.
        */
        ComponentQuery(
                SopranoModule::Ptr soprano,
                ComponentQueryModule::Ptr component,
                const std::string& sparql)
            :
                componentQueryModule_(component),
                soprano_(soprano),
                queryString_(sparql)
        {
        }

        /**
            Returns an extended query with a mapping from the given variable
            (as used in the sparql query!) to the template type 'U'.
        */
        template <class U>
        ComponentQuery<U> with(const std::string& variable)
        {
            return ComponentQuery<U>(variable, *this);
        }

        /**
            Executes the query, returning a vector of tuples.
            The contents of the tuples depend on the variable-type-mappings
            defined using ".with<U>(var)". This base class just returns
            SPARQL results, i.e. a
                std::vector<std::tuple<SPARQLQuery::Bindings>>.
            After using e.g. ".with<GeosGeomI::Ptr>(var)" the result would be
                std::vector<std::tuple<SPARQLQuery::Bindings, GeosGeomI::Ptr>>
        */
        std::vector<ResultType> execute()
        {
            SPARQLQuery query;
            query.query = queryString_;

            // execute sparql query
            soprano_->answer(query);

            std::vector<ResultType> result;
            for (auto& b : query.results)
            {
                result.push_back(std::make_tuple(b));
            }

            return result;
        }

    protected:
        ComponentQueryModule::Ptr componentQueryModule_;

    private:
        SopranoModule::Ptr soprano_;
        std::string queryString_;
    };


    template <class T, class... Ts>
    class ComponentQuery<T, Ts...> : public ComponentQuery<Ts...> {
        friend class ComponentQuery<Ts...>;
    public:
        typedef typename ExtendTuple<
                     typename ComponentQuery<Ts...>::ResultType,
                     ComponentQueryResult<T>>::type
            ResultType;


        template <class U>
        ComponentQuery<U, T, Ts...> with(const std::string& var)
        {
            return ComponentQuery<U, T, Ts...>(var, *this);
        }

        // specify whether the result shall include inferred components
        ComponentQuery<T, Ts...>& includeInferred(bool f)
        {
            includeInferred_ = f;
            return *this;
        }

        ComponentQuery<T, Ts...>& optional(bool f)
        {
            optional_ = f;
            return *this;
        }

        std::vector<ResultType> execute()
        {
            std::vector<ResultType> results;

            auto prevVec = this->ComponentQuery<Ts...>::execute();

            for (auto& prev : prevVec)
            {
                // Keep track if at least one match with the current result from
                // the superclass was found. If optional_ is set we might need
                // to add an empty result in the end.
                bool foundAMatch = false;

                // check if the given variable name was even present in the
                // sparql query
                auto& sparqlResult = std::get<0>(prev);
                if (sparqlResult.find(var_) == sparqlResult.end())
                {
                    // the given variable was not bound - there is a mismatch
                    // between the sparql-query and the variable names given
                    // in the .with<...>(varName) statements.
                    throw std::runtime_error(
                            "Unbound variable - was ?" + var_ +
                            " not used in the sparql query?");
                }

                // get the id from the sparql query and get all ECWMEs that
                // contain an entity with that id
                auto id = sparqlResult[var_].second;
                auto set_of_ecwme = this->componentQueryModule_->get(id);

                // check every ECWME if it matches the criteria
                for (auto& ecwme : set_of_ecwme)
                {
                    auto entity = std::get<0>(ecwme->value_);
                    auto component = std::get<1>(ecwme->value_);
                    auto tag = std::get<2>(ecwme->value_);

                    // NOTE: Components extracted from a ecwme are always shared_ptr
                    auto specific = std::dynamic_pointer_cast<T>(component);

                    // prepare result
                    ComponentQueryResult<T> result;
                    result.component = specific;
                    result.entity = entity;
                    result.tag = tag;

                    // maybe we need to skip this ecwme, if the component is
                    // only inferred
                    if (!includeInferred_ && result.isInferred()) continue;

                    if (specific)
                    {
                        ResultType ext;
                        copy_tuple(prev, ext);
                        std::get<std::tuple_size<ResultType>::value-1>(ext)
                            = result;
                        results.push_back(ext);

                        foundAMatch = true;
                    }
                }

                // if this query-part is marked optional and no match was
                // found, we need to add one with an empty result
                if (!foundAMatch && optional_)
                {
                    ResultType ext;
                    copy_tuple(prev, ext);
                    // note: no values set to the ComponentQueryResult<T> in
                    // the last part of the tuple makes its evaluation as bool
                    // 'false'.
                    results.push_back(ext);
                }
            }

            return results;
        }

    protected:
        ComponentQuery(
                const std::string& var,
                const ComponentQuery<Ts...>& toExtend)
            :
                // use the copy constructor to initialize the base class
                ComponentQuery<Ts...>(toExtend),
                // and just remember the newly added variable name
                var_(var),
                // by default, dont include inferred components
                // (for safety - don't modify them!)
                includeInferred_(false),
                // by default, every "with<T>" is required
                optional_(false)
        {
        }


    private:
        std::string var_; // the variable this class cares about
        bool includeInferred_; // whether to include inferred data in the result
        bool optional_; // if false, results from superclasses are dropped if
                        // no matching component is found.
                        // if true, results from superclasses are added with
                        // a single empty ComponentQueryResult<T> if no match
                        // was found.
                        // Similar to the difference between INNER JOIN and
                        // LEFT JOIN
    };


}


#endif /* include guard: SEMPR_COMPONENTQUERY_HPP_ */
