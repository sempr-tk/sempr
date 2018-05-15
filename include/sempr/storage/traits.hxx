#ifndef SEMPR_TRAITS
#define SEMPR_TRAITS


#if defined(DATABASE_SQLITE)
#  include <sempr/storage/traits/traits-sqlite.hxx>
#elif defined(DATABASE_MYSQL)
#   error no traits for DATABASE_MYSQL
#elif defined(DATABASE_PGSQL)
#   error no traits for DATABASE_PGSQL
#elif defined(DATABASE_ORACLE)
#   error no traits for DATABASE_ORACLE
#elif defined(DATABASE_MSSQL)
#   error no traits for DATABASE_MSSQL
#else
#  error unknown database; did you forget to define the DATABASE_* macros?
#endif


#endif /* end of include guard: SEMPR_TRAITS */
