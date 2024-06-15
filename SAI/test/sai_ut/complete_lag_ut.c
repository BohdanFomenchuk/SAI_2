#include <stdio.h>
#include "sai.h"
#include "sailag.h"

const char* test_profile_get_value(_In_ sai_switch_profile_id_t profile_id, _In_ const char* variable) {
    return NULL;
}

int test_profile_get_next_value(_In_ sai_switch_profile_id_t profile_id, _Out_ const char** variable, _Out_ const char** value) {
    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

#define MAX_LAGS 5
#define MAX_LAG_MEMBERS 16
#define MAX_PORTS 32

void print_status(sai_status_t status, const char* msg) {
    if (status == SAI_STATUS_SUCCESS) {
        printf("%s: SUCCESS\n", msg);
    } else {
        printf("%s: FAILURE (status=%d)\n", msg, status);
    }
}

void print_failure(sai_status_t status, const char* msg) {
    if (status != SAI_STATUS_SUCCESS) {
        printf("%s: SUCCESS (status=%d)\n", msg, status);
    } else {
        printf("%s: FAILURE (expected failure)\n", msg);
    }
}

int main() {
    sai_status_t status;
    sai_lag_api_t* lag_api;
    sai_object_id_t lag_oids[MAX_LAGS];
    sai_object_id_t lag_member_oids[MAX_LAG_MEMBERS];
    sai_attribute_t attr;
    sai_attribute_t attrs[2];
    int i;

    // Initialize SAI API
    status = sai_api_initialize(0, &test_services);
    print_status(status, "SAI API Initialize");
    if (status != SAI_STATUS_SUCCESS) return 1;

    // Query LAG API
    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    print_status(status, "Query LAG API");
    if (status != SAI_STATUS_SUCCESS) return 1;

    // Test Case 1.1: Create 5 LAGs
    for (i = 0; i < MAX_LAGS; i++) {
        status = lag_api->create_lag(&lag_oids[i], 0, NULL);
        char msg[50];
        sprintf(msg, "Create LAG #%d", i + 1);
        print_status(status, msg);
        if (status != SAI_STATUS_SUCCESS) return 1;
    }

    // Test Case 1.2: Attempt to create a 6th LAG (expect failure)
    sai_object_id_t extra_lag_oid;
    status = lag_api->create_lag(&extra_lag_oid, 0, NULL);
    print_failure(status, "Attempt to create 6th LAG");

    // Test Case 1.3: Delete all LAGs and validate removal
    for (i = 0; i < MAX_LAGS; i++) {
        status = lag_api->remove_lag(lag_oids[i]);
        char msg[50];
        sprintf(msg, "Remove LAG #%d", i + 1);
        print_status(status, msg);
        if (status != SAI_STATUS_SUCCESS) return 1;
    }

    // Test Case 1.4: Delete non-existent LAG
    status = lag_api->remove_lag(lag_oids[0]);
    print_failure(status, "Delete non-existent LAG");

    // Test Case 1.5: Create LAG twice
    status = lag_api->create_lag(&lag_oids[0], 0, NULL);
    print_status(status, "Create LAG #1 again");
    status = lag_api->create_lag(&lag_oids[0], 0, NULL);
    print_failure(status, "Create LAG #1 twice");

    // Test Case 2.1: Create 16 LAG members
    int lag_member_count = 0;
    for (i = 0; i < MAX_LAGS && lag_member_count < MAX_LAG_MEMBERS; i++) {
        for (int j = 0; j < MAX_PORTS / MAX_LAGS && lag_member_count < MAX_LAG_MEMBERS; j++) {
            attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
            attrs[0].value.oid = lag_oids[i];
            attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
            attrs[1].value.oid = j + 1;

            status = lag_api->create_lag_member(&lag_member_oids[lag_member_count], 2, attrs);
            char msg[50];
            sprintf(msg, "Create LAG_MEMBER #%d for LAG #%d", lag_member_count + 1, i + 1);
            print_status(status, msg);
            if (status != SAI_STATUS_SUCCESS) return 1;
            lag_member_count++;
        }
    }

    // Test Case 2.2: Attempt to create a 17th LAG member (expect failure)
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag_oids[0];
    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = MAX_PORTS + 1;

    sai_object_id_t extra_lag_member_oid;
    status = lag_api->create_lag_member(&extra_lag_member_oid, 2, attrs);
    print_failure(status, "Attempt to create 17th LAG_MEMBER");

    // Test Case 2.3: Add LAG members to different LAGs and validate
    for (i = 0; i < MAX_LAG_MEMBERS; i++) {
        attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        attrs[0].value.oid = lag_oids[i % MAX_LAGS];
        attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        attrs[1].value.oid = (i % MAX_PORTS) + 1;

        status = lag_api->create_lag_member(&lag_member_oids[i], 2, attrs);
        char msg[50];
        sprintf(msg, "Create LAG_MEMBER #%d for LAG #%d", i + 1, (i % MAX_LAGS) + 1);
        print_status(status, msg);
        if (status != SAI_STATUS_SUCCESS) return 1;
    }

    // Test Case 2.4: Delete LAG members and validate removal
    for (i = 0; i < MAX_LAG_MEMBERS; i++) {
        status = lag_api->remove_lag_member(lag_member_oids[i]);
        char msg[50];
        sprintf(msg, "Remove LAG_MEMBER #%d", i + 1);
        print_status(status, msg);
        if (status != SAI_STATUS_SUCCESS) return 1;
    }

    // Test Case 2.5: Delete non-existent LAG Member
    status = lag_api->remove_lag_member(lag_member_oids[0]);
    print_failure(status, "Delete non-existent LAG_MEMBER");

    // Test Case 2.6: Create LAG Member twice
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag_oids[0];
    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = 1;

    status = lag_api->create_lag_member(&lag_member_oids[0], 2, attrs);
    print_status(status, "Create LAG_MEMBER #1");
    status = lag_api->create_lag_member(&lag_member_oids[0], 2, attrs);
    print_failure(status, "Create LAG_MEMBER #1 twice");

    // Test Case 3.1: Set and get PORT_LIST attribute for LAGs
    for (i = 0; i < MAX_LAGS; i++) {
        attr.id = SAI_LAG_ATTR_PORT_LIST;
        status = lag_api->get_lag_attribute(lag_oids[i], 1, &attr);
        char msg[50];
        sprintf(msg, "Get PORT_LIST for LAG #%d", i + 1);
        print_status(status, msg);
        if (status != SAI_STATUS_SUCCESS) return 1;
    }

    // Test Case 3.2: Set and get PORT_ID and LAG_ID attributes for LAG members
    for (i = 0; i < MAX_LAG_MEMBERS; i++) {
        attr.id = SAI_LAG_MEMBER_ATTR_LAG_ID;
        status = lag_api->get_lag_member_attribute(lag_member_oids[i], 1, &attr);
        char msg[50];
        sprintf(msg, "Get LAG_ID for LAG_MEMBER #%d", i + 1);
        print_status(status, msg);
        if (status != SAI_STATUS_SUCCESS) return 1;

        attr.id = SAI_LAG_MEMBER_ATTR_PORT_ID;
        status = lag_api->get_lag_member_attribute(lag_member_oids[i], 1, &attr);
        sprintf(msg, "Get PORT_ID for LAG_MEMBER #%d", i + 1);
        print_status(status, msg);
        if (status != SAI_STATUS_SUCCESS) return 1;
    }

    // Test Case 4.1: Attempt to add the same port to multiple LAGs (expect failure)
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag_oids[0];
    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = 1;

    status = lag_api->create_lag_member(&lag_member_oids[0], 2, attrs);
    print_status(status, "Create LAG_MEMBER #1 for LAG #1");

    attrs[0].value.oid = lag_oids[1];
    status = lag_api->create_lag_member(&lag_member_oids[1], 2, attrs);
    print_failure(status, "Add same PORT_ID to multiple LAGs");

    // Test Case 4.2: Create LAG member with invalid LAG/PORT IDs (expect failure)
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = (sai_object_id_t)-1;
    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = (sai_object_id_t)-1;

    status = lag_api->create_lag_member(&lag_member_oids[0], 2, attrs);
    print_failure(status, "Create LAG_MEMBER with invalid LAG/PORT IDs");


    printf("All tests executed.\n");

    // Cleanup SAI API
    status = sai_api_uninitialize();
    print_status(status, "SAI API Uninitialize");

    return 0;
}

