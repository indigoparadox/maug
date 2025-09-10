
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
 * \relates MLISP_EXEC_STATE
 * \brief Mask for MLISP_EXEC_STATE::flags to block off flags that persist
 *        between steps.
 */
#define MLISP_EXEC_FLAG_TRANSIENT_MASK 0x0f

/**
 * \relates MLISP_EXEC_STATE
 * \brief Flag for MLISP_EXEC_STATE::flags indicating next token is a term to
 *        be defined.
 */
#define MLISP_EXEC_FLAG_DEF_TERM   0x20

/**
 * \relates MLISP_EXEC_STATE
 * \brief Flag for MLISP_EXEC_STATE::flags indicating defines and calls should
 *        reference MLISP_PARSER::env instead of MLISP_EXEC_STATE::env.
 *
 * This allows multiple MLISP_EXEC_STATE objects to share a common set of
 * definitions.
 */
#define MLISP_EXEC_FLAG_SHARED_ENV 0x40

#define MLISP_EXEC_FLAG_INITIALIZED   0x08

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

/**
 * \brief A callback to attach to an mlisp command with mlisp_env_set() with
 *        MLISP_TYPE_CB.
 * \param args_c The number of top-level arguments provided to this callback
 *               when it was called.
 */
typedef MERROR_RETVAL (*mlisp_env_cb_t)(
   struct MLISP_PARSER* parser, struct MLISP_EXEC_STATE* exec, size_t n_idx,
   size_t args_c, uint8_t* cb_data, uint8_t flags );

#define _MLISP_TYPE_TABLE_FIELDS( idx, ctype, name, const_name, fmt ) \
   ctype name;

/* We need to ignore this since it builds the fields with the preprocessor and
 * the meta-serializer doesn't support that.
 */
/* mserial_ignore_until_next_cbrace */
union MLISP_VAL {
   MLISP_TYPE_TABLE( _MLISP_TYPE_TABLE_FIELDS )
};

struct MLISP_ENV_NODE {
   uint8_t flags;
   uint8_t type;
   mdata_strpool_idx_t name_strpool_idx;
   size_t name_strpool_sz;
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

/**
 * \brief Current execution state to associate with a MLISP_PARSER.
 *
 * \note In general, this library uses nested locks because MLISP_STACK_NODE and
 *       MLISP_ENV_NODE do not have MDATA_VECTOR structs attached, so there's
 *       no danger of a MDATA_VECTOR being reallocated out from beneath another.
 */
struct MLISP_EXEC_STATE {
   /* serial_prepare_cb mlisp_deserialize_prepare_EXEC_STATE */
   uint16_t uid;
   /*! \brief Flags which dictate the behavior of this object. */
   uint8_t flags;
   /*! \brief The number of times each node has been visited ever. */
   /* vector_type size_t */
   struct MDATA_VECTOR per_node_visit_ct;
   /**
    * \brief The hild index that will be visited on next visit of each node.
    *
    * This is tracked per MLISP_AST_NODE, so each node has its own "program
    * counter." This facilitates things like tail call optimization.
    */
   /* vector_type size_t */
   struct MDATA_VECTOR per_node_child_idx;
   /*! \brief A stack of data values resulting from evaluating statements. */
   /* vector_type struct MLISP_STACK_NODE */
   struct MDATA_VECTOR stack;
   /**
    * \brief Environment in which statements are defined if ::MLISP_
    *
    * This is segmented with ::MLISP_TYPE_ARGS_S and :: MLISP_TYPE_ARGS_E, to
    * denote env definitions that are actually args for the current lambda.
    */
   /* vector_type struct MLISP_ENV_NODE */
   struct MDATA_VECTOR env;
   /*! \brief Dummy field; do not serialize fields after this! */
   int8_t no_serial;
/**
 * \brief Path through any lambdas the execution has entered during *this*
 *        heartbeat cycle. Used to detect tail calls.
 */
   /* vector_type size_t */
   struct MDATA_VECTOR lambda_trace;
   void* cb_attachment;
#ifdef MLISP_DEBUG_TRACE
   size_t trace[MLISP_DEBUG_TRACE];
   size_t trace_depth;
#endif /* MLISP_DEBUG_TRACE */
   /* vector_type struct MLISP_ENV_NODE */
   struct MDATA_VECTOR* global_env;
};

struct MLISP_PARSER {
   struct MPARSER base;
   struct MDATA_STRPOOL strpool;
   struct MDATA_VECTOR ast;
   /**
    * \brief Definitions to use if ::MLISP_EXEC_FLAG_DEF_TERM is defined on the
    *        accompanying MLISP_EXEC_STATE::flags.
    */
   struct MDATA_VECTOR env;
   ssize_t ast_node_iter;
};

/*! \} */ /* mlisp */

#endif /* !MLISPS_H */

