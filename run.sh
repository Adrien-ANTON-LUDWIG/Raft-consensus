#!/bin/sh


client_count=2
server_count=3
build_dir="build"
exec="my_aws_s3"

echo $exec

print_help()
{
    echo "Usage: $0 [flags]"
    echo
    echo -e 'Flags:'
    echo '-c, --client_n Set client count. Default to '"${client_count}."
    echo '-s, --server_n Set server count. Default to '"${server_count}."
}

while [ $# -gt 0 ]; do
    case "$1" in
        -h|--help)
            print_help
            exit 0
            ;;

        -c|--client_n)
            shift
            client_count="$1"
            shift
            ;;
        -s|--server_n)
            shift
            server_count="$1"
            shift
            ;;
    esac
done

total_size=$(($client_count + $server_count + 1))

mpiexec -np ${total_size} --oversubscribe ./"${exec}" "${client_count}" "${server_count}"