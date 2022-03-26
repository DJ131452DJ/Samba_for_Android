#!/bin/sh

. "${TEST_SCRIPTS_DIR}/unit.sh"

define_test "enable default (0)"

setup_ctdbd <<EOF
NODEMAP
0       192.168.20.41   0x4     CURRENT RECMASTER
1       192.168.20.42   0x0
2       192.168.20.43   0x0
EOF

ok_null
simple_test

ok <<EOF
Number of nodes:3
pnn:0 192.168.20.41    OK (THIS NODE)
pnn:1 192.168.20.42    OK
pnn:2 192.168.20.43    OK
EOF
simple_test_other nodestatus all
