export CMAKE_BUILD_PARALLEL_LEVEL := `nproc --all`

_list:
	@just --list

format:
	find src -name '*xx' -print0 | xargs -0 clang-format -i --style file

test:
	mkdir -p build
	cmake -Bbuild -S. -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE
	cp build/compile_commands.json .
	cmake --build build
	# ctest --build-and-test . build --build-generator 'Unix Makefiles'
	ctest --test-dir build
