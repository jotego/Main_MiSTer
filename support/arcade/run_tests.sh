#!/bin/bash -e

main() {
	clean_up_on_exit
	build_test
	run_test
	pass
}

clean_up_on_exit() {
	trap clean_up INT KILL EXIT
}

clean_up() {
	rm -f rom_data_test
}

build_test() {
	g++ rom_data_test.cpp -o rom_data_test || fail
}

run_test() {
	rom_data_test || fail
}

fail() {
	echo FAIL
	exit 1
}

pass() {
	echo PASS
	exit 0
}

main "$@"
