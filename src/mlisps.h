
#ifndef MLISPS_H
#define MLISPS_H

/**
 * \addtogroup mlisp MLISP Interpreter
 * \{
 *
 * \file mlisps.h
 * \brief MLISP Interpreter/Parser Structs
 */

#ifndef MLISP_TRACE_LVL
#  define MLISP_TRACE_LVL 0
#endif /* !MLISP_TRACE_LVL */

#ifndef MLISP_TRACE_SIGIL
#  define MLISP_TRACE_SIGIL "TRACE"
#endif /* !MLISP_TRACE_SIGIL */

#ifndef MLISP_AST_IDX_CHILDREN_MAX
#  define MLISP_AST_IDX_CHILDREN_MAX 10
#endif /* !MLISP_AST_IDX_CHILDREN_MAX */

/**
 * \addtogroup mlisp_types MLISP Types
 * \{
 */

typedef ssize_t mlisp_lambda_t;

typedef mlisp_lambda_t mlisp_args_t;
typedef mlisp_lambda_t mlisp_arge_t;
typedef mlisp_lambda_t mlisp_if_t;
typedef mlisp_lambda_t mlisp_begin_t;

typedef uint8_t mlisp_bool_t;

/**
 * \brief Table of numeric types.
 *
 * These are carved out because they can usually be handled in a similar way.
 */
#define MLISP_NUM_TYPE_TABLE( f ) \
   f( 1, int16_t,             integer,         INT,      "%d" ) \
   f( 2, float,               floating,        FLOAT,    "%f" ) \
   f( 3, mlisp_bool_t,        boolean,         BOOLEAN,  "%u" )

/**
 * \brief Table of other types.
 *
 * These are special cases that must usually be handled manually.
 */
#define MLISP_TYPE_TABLE( f ) \
   MLISP_NUM_TYPE_TABLE( f ) \
   f( 4, mdata_strpool_idx_t, strpool_idx,   STR,     SSIZE_T_FMT ) \
   f( 5, mlisp_env_cb_t,      cb,            CB,      "%p" ) \
   f( 6, mlisp_lambda_t,      lambda,        LAMBDA,  SSIZE_T_FMT ) \
   f( 7, mlisp_args_t,        args_start,    ARGS_S,  SSIZE_T_FMT ) \
   f( 8, mlisp_arge_t,        args_end,      ARGS_E,  SSIZE_T_FMT ) \
   f(10, mlisp_begin_t,       begin,         BEGIN,   SSIZE_T_FMT )   

/*! \} */ /* mlisp_types */

struct MLISP_PARSER;
struct MLISP_EXEC_STATE;

typedef MERROR_RETVAL (*mlisp_env_cb_t)(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   void* cb_data, uint8_t flags );

#define _MLISP_TYPE_TABLE_FIELDS( idx, ctype, name, const_name, fmt ) \
   ctype name;

union MLISP_VAL {
   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_FIELDS );
};

struct MLISP_ENV_NODE {
   uint8_t flags;
   uint8_t type;
   mdata_strpool_idx_t name_strpool_idx;
   union MLISP_VAL value;
   void* cb_data;
};

struct MLISP_STACK_NODE {
   uint8_t type;
   union MLISP_VAL value;
};

struct MLISP_AST_NODE {
   uint8_t flags;
   mdata_strpool_idx_t token_idx;
   size_t token_sz;
   ssize_t ast_idx_parent;
   ssize_t env_idx_op;
   ssize_t ast_idx_children[MLISP_AST_IDX_CHILDREN_MAX];
   /*! \brief Number of children in MLISP_AST_NODE::ast_idx_children. */
   size_t ast_idx_children_sz;
};

struct MLISP_EXEC_STATE {
   uint8_t flags;
   /*! \brief The number of times each node has been visited ever. */
   struct MDATA_VECTOR per_node_visit_ct;
   struct MDATA_VECTOR per_node_child_idx;
   /*! \brief A stack of data values resulting from evaluating statements. */
   struct MDATA_VECTOR stack;
   /**
    * \brief Environment in which statements are defined.
    *
    * This is segmented with ::MLISP_TYPE_ARGS_S and :: MLISP_TYPE_ARGS_E, to
    * denote env definitions that are actually args for the current lambda.
    */
   struct MDATA_VECTOR env;
/**
 * \brief Path through any lambdas the execution has entered during *this*
 *        heartbeat cycle. Used to detect tail calls.
 */
   struct MDATA_VECTOR lambda_trace;
   void* cb_attachment;
#ifdef MLISP_DEBUG_TRACE
   size_t trace[MLISP_DEBUG_TRACE];
   size_t trace_depth;
#endif /* MLISP_DEBUG_TRACE */
};

struct MLISP_PARSER {
   struct MPARSER base;
   struct MDATA_STRPOOL strpool;
   struct MDATA_VECTOR ast;
   ssize_t ast_node_iter;
};

/*! \} */ /* mlisp */

#endif /* !MLISPS_H */

