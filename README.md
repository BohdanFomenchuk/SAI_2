# To run LAG unit-test do net steps:

## 1. Build SAI
cd stub
./autogen.sh
./configure --prefix=/usr
make && sudo make install

## 2. Compile lag_test.c (2-nd Home Work) and complete_lag_ut.c (3-rd Home Work) 
cd SAI/test/sai_ut
gcc -o lag_unit_test -I /usr/include/sai -L /usr/lib lag_test.c -lsai
gcc -o complete_lag_unit_test -I /usr/include/sai -L /usr/lib comlete_lag_ut.c -lsai 

## 3. Run tests
./lag_unit_test
./complete_lag_unit_test

## 4. Veirfy output (lag_unit_test output example)
CREATE LAG: 0x100000002
CREATE LAG MEMBER: 0x100000019
CREATE LAG MEMBER: 0x200000019
CREATE LAG: 0x200000002
CREATE LAG MEMBER: 0x300000019
CREATE LAG MEMBER: 0x400000019
GET LAG ATTRIBUTE: 0x100000002
GET LAG ATTRIBUTE: 0x200000002
GET LAG MEMBER ATTRIBUTE: 0x100000019
GET LAG MEMBER ATTRIBUTE: 0x300000019
REMOVE LAG MEMBER: 0x200000019
GET LAG ATTRIBUTE: 0x100000002
REMOVE LAG MEMBER: 0x300000019
GET LAG ATTRIBUTE: 0x200000002
REMOVE LAG MEMBER: 0x100000019
REMOVE LAG MEMBER: 0x400000019
REMOVE LAG: 0x200000002
REMOVE LAG: 0x100000002

# Complete SAI LAG API Test Plan

## Objective
Validate the SAI LAG API implementation against specified requirements.

## Requirements
1. Up to 5 LAGs.
2. Up to 16 LAG_MEMBERs.
3. Up to 32 PORTs.
4. Supported LAG attributes: PORT_LIST.
5. Supported LAG_MEMBER attributes: PORT_ID, LAG_ID.

## Test Cases

### 1. LAG Creation and Deletion
- Test Case 1.1: Create 5 LAGs.
- Test Case 1.2: Attempt to create a 6th LAG (expect failure).
- Test Case 1.3: Delete all LAGs and validate removal.
- Test Case 1.4: Delete non-existed LAG
- Test Case 1.5: Create LAG twise

### 2. LAG_MEMBER Creation and Deletion
- Test Case 2.1: Create 16 LAG members.
- Test Case 2.2: Attempt to create a 17th LAG member (expect failure).
- Test Case 2.3: Add LAG members to different LAGs and validate.
- Test Case 2.4: Delete LAG members and validate removal.
- Test Case 2.5: Delete non-existed LAG Member
- Test Case 2.6: Create LAG Member twise

### 3. Attribute Setting and Getting
- Test Case 3.1: Set and get PORT_LIST attribute for LAGs.
- Test Case 3.2: Set and get PORT_ID and LAG_ID attributes for LAG members.

### 4. Invalid Operations
- Test Case 4.1: Attempt to add the same port to multiple LAGs (expect failure).
- Test Case 4.2: Create LAG member with invalid LAG/PORT IDs (expect failure).

### 5. Stress Testing
- Test Case 5.1: Create, modify, and delete LAGs and LAG members under boundary conditions.

## Execution
- Execute tests in the specified order.
- Log results and validate against expected outcomes.
- Report any discrepancies and issues for further investigation.

