#include "../../../internal/cryptoutil/src/keys.h"
#include "../../../internal/jwtutil/src/util.h"
#include "../../../spiffeid/src/trustdomain.h"
#include "../src/set.h"
#include <check.h>
#include <openssl/pem.h>

/*
Each test named 'test_jwtbundle_<function name>' tests
jwtbundle_<function name> function.
*/

// precondition: valid jwt bundle objects
// postcondition: valid jwt bundle set with its
// respective bundles
START_TEST(test_jwtbundle_NewSet)
{
    const int ITERS = 3;

    spiffeid_TrustDomain td[]
        = { { "example1.com" }, { "example2.com" }, { "example3.com" } };

    err_t err;
    jwtbundle_Bundle *bundle_ptr[ITERS];

    for(int i = 0; i < ITERS; ++i) {
        bundle_ptr[i]
            = jwtbundle_Load(td[i], "./resources/jwk_keys.json", &err);
        ck_assert_uint_eq(err, NO_ERROR);
    }

    jwtbundle_Set *set
        = jwtbundle_NewSet(ITERS, bundle_ptr[0], bundle_ptr[1], bundle_ptr[2]);

    ck_assert_uint_eq(shlenu(set->bundles), ITERS);

    for(int i = 0; i < ITERS; ++i) {
        ck_assert_int_ge(shgeti(set->bundles, td[i].name), 0);
    }

    jwtbundle_Set_Free(set);
}
END_TEST

// precondition: valid empty jwt set object
// postcondition: valid jwt bundle set with after
// each function call
START_TEST(test_jwtbundle_Set_Add)
{
    const int ITERS = 3;

    spiffeid_TrustDomain td[]
        = { { "example1.com" }, { "example2.com" }, { "example3.com" } };

    err_t err;
    jwtbundle_Bundle *bundle_ptr[ITERS];

    for(int i = 0; i < ITERS; ++i) {
        bundle_ptr[i]
            = jwtbundle_Load(td[i], "./resources/jwk_keys.json", &err);
        ck_assert_uint_eq(err, NO_ERROR);
    }

    jwtbundle_Set *set = jwtbundle_NewSet(0);

    for(int i = 0; i < ITERS; ++i) {
        jwtbundle_Set_Add(set, bundle_ptr[i]);
    }

    ck_assert_uint_eq(shlenu(set->bundles), ITERS);

    for(int i = 0; i < ITERS; ++i) {
        ck_assert_int_ge(shgeti(set->bundles, td[i].name), 0);
    }

    jwtbundle_Set_Free(set);
}
END_TEST

// precondition: valid jwt set object
// postcondition: valid jwt bundle set with after
// each function call
START_TEST(test_jwtbundle_Set_Remove)
{
    const int ITERS = 3;

    spiffeid_TrustDomain td[]
        = { { "example1.com" }, { "example2.com" }, { "example3.com" } };

    err_t err;
    jwtbundle_Bundle *bundle_ptr[ITERS];

    for(int i = 0; i < ITERS; ++i) {
        bundle_ptr[i]
            = jwtbundle_Load(td[i], "./resources/jwk_keys.json", &err);
        ck_assert_uint_eq(err, NO_ERROR);
    }

    jwtbundle_Set *set
        = jwtbundle_NewSet(ITERS, bundle_ptr[0], bundle_ptr[1], bundle_ptr[2]);

    for(int i = 0; i < ITERS; ++i) {
        ck_assert_int_ge(shgeti(set->bundles, td[i].name), 0);
    }

    for(int i = 0; i < ITERS; ++i) {
        jwtbundle_Set_Remove(set, td[i]);
        ck_assert_int_lt(shgeti(set->bundles, td[i].name), 0);
    }

    jwtbundle_Set_Free(set);
}
END_TEST

// precondition: valid jwt set object
// postcondition: valid result for each query
START_TEST(test_jwtbundle_Set_Has)
{
    const int ITERS = 3;

    spiffeid_TrustDomain td[]
        = { { "example1.com" }, { "example2.com" }, { "example3.com" } };

    err_t err;
    jwtbundle_Bundle *bundle_ptr[ITERS];

    for(int i = 0; i < ITERS; ++i) {
        bundle_ptr[i]
            = jwtbundle_Load(td[i], "./resources/jwk_keys.json", &err);
        ck_assert_uint_eq(err, NO_ERROR);
    }

    jwtbundle_Set *set = jwtbundle_NewSet(0);

    for(int i = 0; i < ITERS; ++i) {
        ck_assert(!jwtbundle_Set_Has(set, td[i]));
    }

    for(int i = 0; i < ITERS; ++i) {
        jwtbundle_Set_Add(set, bundle_ptr[i]);
    }

    for(int i = 0; i < ITERS; ++i) {
        ck_assert(jwtbundle_Set_Has(set, td[i]));
    }

    jwtbundle_Set_Free(set);
}
END_TEST

// precondition: valid jwt set object
// postcondition: valid result for each query
START_TEST(test_jwtbundle_Set_Get)
{
    const int ITERS = 3;

    spiffeid_TrustDomain td[]
        = { { "example1.com" }, { "example2.com" }, { "example3.com" } };

    err_t err;
    jwtbundle_Bundle *bundle_ptr[ITERS];

    for(int i = 0; i < ITERS; ++i) {
        bundle_ptr[i]
            = jwtbundle_Load(td[i], "./resources/jwk_keys.json", &err);
        ck_assert_uint_eq(err, NO_ERROR);
    }

    jwtbundle_Set *set
        = jwtbundle_NewSet(ITERS, bundle_ptr[0], bundle_ptr[1], bundle_ptr[2]);

    for(int i = 0; i < ITERS; ++i) {
        bool suc;
        jwtbundle_Bundle *b = jwtbundle_Set_Get(set, td[i], &suc);

        ck_assert(suc);
        ck_assert(b != NULL);
        ck_assert_uint_eq(shlenu(b->auths), 3);
        ck_assert_int_eq(strcmp(b->td.name, td[i].name), 0);
    }

    spiffeid_TrustDomain newtd = { "example4.com" };
    bool suc;
    jwtbundle_Bundle *b = jwtbundle_Set_Get(set, newtd, &suc);

    ck_assert(!suc);
    ck_assert(b == NULL);

    jwtbundle_Set_Free(set);
}
END_TEST

// precondition: valid jwt set object
// postcondition: valid array of jwt bundles
// corresponding to the bundles store in the set
START_TEST(test_jwtbundle_Set_Bundles)
{
    const int ITERS = 3;

    spiffeid_TrustDomain td[]
        = { { "example1.com" }, { "example2.com" }, { "example3.com" } };

    err_t err;
    jwtbundle_Bundle *bundle_ptr[ITERS];

    for(int i = 0; i < ITERS; ++i) {
        bundle_ptr[i]
            = jwtbundle_Load(td[i], "./resources/jwk_keys.json", &err);
        ck_assert_uint_eq(err, NO_ERROR);
    }

    jwtbundle_Set *set
        = jwtbundle_NewSet(ITERS, bundle_ptr[0], bundle_ptr[1], bundle_ptr[2]);

    jwtbundle_Bundle **bundles = jwtbundle_Set_Bundles(set);

    ck_assert_uint_eq(arrlenu(bundles), ITERS);

    for(int i = 0; i < ITERS; ++i) {
        ck_assert_int_eq(strcmp(bundles[i]->td.name, td[i].name), 0);
    }

    jwtbundle_Set_Free(set);
}
END_TEST

// precondition: valid jwt set object
// postcondition: correc size of the set
START_TEST(test_jwtbundle_Set_Len)
{
    const int ITERS = 3;

    spiffeid_TrustDomain td[]
        = { { "example1.com" }, { "example2.com" }, { "example3.com" } };

    err_t err;
    jwtbundle_Bundle *bundle_ptr[ITERS];

    for(int i = 0; i < ITERS; ++i) {
        bundle_ptr[i]
            = jwtbundle_Load(td[i], "./resources/jwk_keys.json", &err);
        ck_assert_uint_eq(err, NO_ERROR);
    }

    jwtbundle_Set *set
        = jwtbundle_NewSet(ITERS, bundle_ptr[0], bundle_ptr[1], bundle_ptr[2]);

    const uint32_t setlen = jwtbundle_Set_Len(set);
    ck_assert_uint_eq(setlen, ITERS);

    jwtbundle_Set_Free(set);
}
END_TEST

// precondition: valid jwt set object
// postcondition: correct bundle for given trust domain
START_TEST(test_jwtbundle_Set_GetJWTBundleForTrustDomain)
{
    const int ITERS = 3;

    spiffeid_TrustDomain td[]
        = { { "example1.com" }, { "example2.com" }, { "example3.com" } };

    err_t err;
    jwtbundle_Bundle *bundle_ptr[ITERS];

    for(int i = 0; i < ITERS; ++i) {
        bundle_ptr[i]
            = jwtbundle_Load(td[i], "./resources/jwk_keys.json", &err);
        ck_assert_uint_eq(err, NO_ERROR);
    }

    jwtbundle_Set *set
        = jwtbundle_NewSet(ITERS, bundle_ptr[0], bundle_ptr[1], bundle_ptr[2]);

    for(int i = 0; i < ITERS; ++i) {
        jwtbundle_Bundle *b
            = jwtbundle_Set_GetJWTBundleForTrustDomain(set, td[i], &err);

        ck_assert_uint_eq(err, NO_ERROR);
        ck_assert(b != NULL);
        ck_assert_uint_eq(shlenu(b->auths), 3);
        ck_assert_int_eq(strcmp(b->td.name, td[i].name), 0);
    }

    spiffeid_TrustDomain newtd = { "example4.com" };
    jwtbundle_Bundle *b
        = jwtbundle_Set_GetJWTBundleForTrustDomain(set, newtd, &err);

    ck_assert_uint_ne(err, NO_ERROR);
    ck_assert(b == NULL);

    jwtbundle_Set_Free(set);
}
END_TEST

Suite *set_suite(void)
{
    Suite *s = suite_create("set");
    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_jwtbundle_NewSet);
    tcase_add_test(tc_core, test_jwtbundle_Set_Add);
    tcase_add_test(tc_core, test_jwtbundle_Set_Remove);
    tcase_add_test(tc_core, test_jwtbundle_Set_Has);
    tcase_add_test(tc_core, test_jwtbundle_Set_Get);
    tcase_add_test(tc_core, test_jwtbundle_Set_Bundles);
    tcase_add_test(tc_core, test_jwtbundle_Set_Len);
    tcase_add_test(tc_core, test_jwtbundle_Set_GetJWTBundleForTrustDomain);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    Suite *s = set_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    const int number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}