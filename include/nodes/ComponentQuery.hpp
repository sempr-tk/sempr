#ifndef SEMPR_COMPONENTQUERY_HPP_
#define SEMPR_COMPONENTQUERY_HPP_

#include <vector>
#include <type_traits>
#include <utility>
#include <tuple>
#include <array>

#include "nodes/SopranoModule.hpp" // for SPARQL-Queries etc
#include "nodes/ComponentQueryNode.hpp"


namespace sempr {

    namespace util {
        // copy the contents of one tuple into a larger one
        // e.g., tuple<int, float> into tuple<int, float, string>
        template <size_t... I, typename T1, typename T2>
        void copy_tuple_impl(T1 const& from, T2& to, std::index_sequence<I...>)
        {
            // The index sequence is just a helper to get a multiple size_t
            // template args in "size_t... I", ranging from 0 to size-1 of the
            // "from" tuple.
            //
            // We need the brace initialization of the dummy-array just to
            // provide a context in which parameter pack expansion is allowed.
            int dummy[] = {
                // The first part is the copy from the source tuple to the
                // destination tuple. The comma-*operator* discards the result
                // of the first expression and always returns the result of the
                // second expression, which in this case is always 0 and
                // provides a valid type for the int-array.
                (std::get<I>(to) = std::get<I>(from), 0)...
                // The ellipsis (...) expands the parameter pack with the given
                // pattern, where the pack (here: I) is replaced by its values:
                // (std::get<0>(to) = std::get<0>(from), 0),
                // (std::get<1>(to) = std::get<1>(from), 0),
                //                [...]
            };
            // the static cast simply suppresses the compiler warning us about
            // the unused variable "dummy".
            static_cast<void>(dummy);
        }

        template <typename T1, typename T2>
        void copy_tuple(T1 const& from, T2& to)
        {
            copy_tuple_impl(from, to,
                std::make_index_sequence<std::tuple_size<T1>::value>());
        }

        /**
            Define a new tuple with more types.
            ExtendTuple<std::tuple<int, float>, float, string, bool>::type
                      = std::tuple<int, float, float, string, bool>
        */
        template <class...> struct ExtendTuple;
        template <class... Ts, class... Us>
        struct ExtendTuple<std::tuple<Ts...>, Us...> {
            typedef std::tuple<Ts..., Us...> type;
        };


        /**
            Return a sub-array from the given array.
        */
        template <size_t Begin, size_t End, class T, size_t N>
        std::array<T, End-Begin> sub_array(const std::array<T, N>& in)
        {
            std::array<T, End-Begin> out;
            std::copy(in.begin() + Begin, in.begin() + End, out.begin());
            return out;
        }
    }


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
            //std::array<std::string, 1> nVars({variable});
            //return ComponentQuery<U>(nVars);
            //
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
        typedef typename util::ExtendTuple<
            typename ComponentQuery<Ts...>::ResultType, std::shared_ptr<T>>::type
            ResultType;


        template <class U>
        ComponentQuery<U, T, Ts...> with(const std::string& var)
        {
            /*
            std::array<std::string, sizeof...(Ts)+2> nVars;
            nVars[0] = var;
            std::copy(vars_.begin(), vars_.end(), nVars.begin()+1);
            return ComponentQuery<U, T, Ts...>(nVars);
            */

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
                    // id matches already (see above: ->get(id))
                    // now dynamic_cast for the type. ... dynamic_cast?
                    // dynamic_pointer_cast? This could get difficult... TODO
                    auto component = std::get<1>(ecwme->value_);
                    auto tag = std::get<2>(ecwme->value_); // TODO: use

                    auto specific = std::dynamic_pointer_cast<T>(component);
                    if (specific)
                    {
                        ResultType ext;
                        util::copy_tuple(prev, ext);
                        std::get<std::tuple_size<ResultType>::value-1>(ext)
                            = specific;
                        results.push_back(ext);
                    }
                }

            }

            return results;
        }

    protected:
        // only construct through ComponentQuery<>("..").with<...>("...")
        //
        // Takes all relevant variable names, takes the first for itself and
        // hands the rest to the super-class.
        /*
        ComponentQuery(std::array<std::string, sizeof...(Ts)+1> variables)
            :
                ComponentQuery<Ts...>(
                    util::sub_array<1, sizeof...(Ts)+1>(variables)
                ),
                var_(variables[0]),
                vars_(variables)
        {
        }
        */

        ComponentQuery(
                const std::string& var,
                const ComponentQuery<Ts...>& toExtend)
            :
                ComponentQuery<Ts...>(toExtend),
                var_(var)
        {
        }


    private:
        std::string var_; // the variable this class cares about

        // the variables this and all subclasses care about.
        // (needed when extending the query using ".with<T>(newVar)")
        /*
        std::array<std::string, sizeof...(Ts)+1> vars_;
        */
    };


}


#endif /* include guard: SEMPR_COMPONENTQUERY_HPP_ */
