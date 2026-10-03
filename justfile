build:
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target gb

format:
    clang-format -i include/**.h src/**.c

check:
    clang-tidy include/**.h src/**.c

fix:
    clang-tidy --fix include/**.h src/**.c

run:
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target gb
    ./build/gb

run_debug path:
    cmake -B build_debug -DCMAKE_BUILD_TYPE=Debug
    cmake --build build_debug --target gb
    ./build_debug/gb {{ path }}

clean:
    rm -rf build
    rm -rf build_debug

build_test:
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DGB_TEST=ON
    cp build/compile_commands.json .
    cmake --build build --target gb

gen_opcode:
    uv run ./codegen/gen.py

test path num time: build_test
    timeout --signal=SIGINT {{ time }} ./build/gb '{{ path }}' || true
    uv run tests/gameboy-doctor/gameboy-doctor ./logs/gameboy_doctor.log cpu_instrs {{ num }}

test01: (test './tests/gb-test-roms/cpu_instrs/individual/01-special.gb' '1' '2') # passed
test02: (test './tests/gb-test-roms/cpu_instrs/individual/02-interrupts.gb' '2' '10')
test03: (test './tests/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb' '3' '2') # passed
test04: (test './tests/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb' '4' '2') # passed
test05: (test './tests/gb-test-roms/cpu_instrs/individual/05-op rp.gb' '5' '2') # passed
test06: (test './tests/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb' '6' '2') # passed
test07: (test './tests/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb' '7' '2') # passed
test08: (test './tests/gb-test-roms/cpu_instrs/individual/08-misc instrs.gb' '8' '2') # passed
test09: (test './tests/gb-test-roms/cpu_instrs/individual/09-op r,r.gb' '9' '4') # passed
test10: (test './tests/gb-test-roms/cpu_instrs/individual/10-bit ops.gb' '10' '30') # passed
test11: (test './tests/gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb' '11' '30') # passed

test_all: test01 test03 test04 test05 test06 test07 test08 test09 test10 test11
