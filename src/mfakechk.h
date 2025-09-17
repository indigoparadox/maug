
#ifndef MFAKECHK_H
#define MFAKECHK_H

#define FAKECHECK_CASES_CT_MAX 10

#define FAKECHECK_SUITES_CT_MAX 10

#define CK_VERBOSE 0x01

typedef void (*fakechk_fn)( int _i );

#define START_TEST( f ) void f ( int _i )

#define END_TEST

#define ck_assert_int_eq( a, b )

#define ck_assert_uint_eq( a, b )

typedef struct {
   fakechk_fn fn;
} TCase;

typedef struct {
   TCase cases[FAKECHECK_CASES_CT_MAX];
} Suite;

typedef struct {
   Suite suites[FAKECHECK_SUITES_CT_MAX];
} SRunner;

Suite* suite_create( const char* name );

SRunner* srunner_create( Suite* suite );

TCase* tcase_create( const char* name );

void suite_add_tcase( Suite* suite, TCase* tc );

void tcase_add_test( TCase* case_struct, fakechk_fn fn );

int srunner_run_all( SRunner* runner, unsigned char flags );

#endif /* !MFAKECHK_H */

