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
        ComponentQuery(std::array<std::string, 0> /*vars*/)
        {
        }

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

        std::vector<ResultType> execute()
        {
            std::vector<ResultType> results;

            auto prevVec = this->ComponentQuery<Ts...>::execute();

            for (auto& prev : prevVec)
            {
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

                auto id = sparqlResult[var_].second;
                auto set_of_ecwme = this->componentQueryModule_->get(id);

                for (auto& ecwme : set_of_ecwme)
                {
                    auto entity = std::get<0>(ecwme->value_);
                    auto component = std::get<1>(ecwme->value_);
                    auto tag = std::get<2>(ecwme->value_); // TODO: use

                    // NOTE: Components extracted from a ecwme are always shared_ptr
                    auto specific = std::dynamic_pointer_cast<T>(component);

                    // prepare result
                    ComponentQueryResult<T> result;
                    result.component = specific;
                    result.entity = entity;
                    result.tag = tag;

                    if (specific)
                    {
                        ResultType ext;
                        copy_tuple(prev, ext);
                        std::get<std::tuple_size<ResultType>::value-1>(ext)
                            = result;
                        results.push_back(ext);
                    }
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
                var_(var)
        {
        }


    private:
        std::string var_; // the variable this class cares about
    };


}


#endif /* include guard: SEMPR_COMPONENTQUERY_HPP_ */
