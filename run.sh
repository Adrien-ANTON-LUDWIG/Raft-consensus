#!/bin/sh


client_count=2
server_count=3
build_dir='build'
exec='my_aws_s3'
client_commands=''
show_logs=0
force_cli=0

print_help()
{
    echo "Usage: $0 [flags]"
    echo
    echo -e 'Flags:'
    echo '-c, --client_n  Set client count. Default to '"${client_count}."
    echo '-s, --server_n  Set server count. Default to '"${server_count}."
    echo '--commands      Set the command file to execute. Default to none.'
    echo '-l, --show_logs Show application logs after exiting.'
    echo '--cli           Force CLI mode for REPL.'
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
        --commands)
            shift
            client_commands="$1"
            shift
            ;;
        -l|--show_logs)
            show_logs=1
            shift
            ;;
        --cli)
            force_cli=1
            shift
            ;;
    esac
done

repl_idx=$(($client_count + $server_count))
total_size=$(($repl_idx + 1))

mpiexec --mca opal_warn_on_missing_libcuda 0 -np ${total_size} --stdin ${repl_idx} --oversubscribe ./"${build_dir}"/"${exec}" "${client_count}" "${server_count}" "${force_cli}" "${client_commands}"

if [ $show_logs -eq 1 ]
then
    echo "Displaying logs"
    cat logs/global_logs.log
fi