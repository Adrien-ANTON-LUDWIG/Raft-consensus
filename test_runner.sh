#!/bin/bash

COK='\033[0;32m'
CKO='\033[0;31m'
NC='\033[0m'

build_dir='build'
exec='my_aws_s3'
test_dir='tests'
runtime_dir='logs'

tmp_test_out=$(mktemp /tmp/sorted_test.XXXXXX)
tmp_log_out=$(mktemp /tmp/sorted_log.XXXXXX)

for testfile in ${test_dir}/*.in
do
    test_name="undefined"
    if [[ "${testfile}" =~ ([^/\.]+)\..*$ ]]
    then
        test_name="${BASH_REMATCH[1]}"
    fi
    test_out=$(echo "${testfile}" | grep -o '^[^.]\+')'.out'
    if [ ! -f ${test_out} ]
    then
        continue
    fi

    sort ${test_out} > ${tmp_test_out}

    echo -ne "Running ${test_name}...\r"
    eval ./run.sh --commands "${testfile}" > /dev/null

    isFailed=0
    all_diffs=""

    for logfile in $(find ${runtime_dir} -name "log_server_*")
    do
        sort ${logfile} > ${tmp_log_out}
        diff_out=$(diff -u ${tmp_test_out} ${tmp_log_out})
        if [ ! -z  "${diff_out}" ]
        then
            isFailed=1
            all_diffs="${all_diffs}"'\n'"$(echo -e "${diff_out}")"'\n'
        fi
    done

    if [ -z "${diff_out}" ]
    then
        echo -e "\e[K${COK}${test_name} success${NC}"
    else
        echo -e "\e[K${CKO}${test_name} failed${NC}"
        echo -e "${all_diffs}"
    fi
done