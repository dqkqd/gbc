build:
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target gb

compile_json:
    cmake -B build_debug -DCMAKE_BUILD_TYPE=Debug -DGB_TEST=ON -DGB_RENDER_TEST=ON
    cp build_debug/compile_commands.json .

format:
    clang-format -i $(git ls-files '*.c' '*.h')

# headers are checked through the translation units (HeaderFilterRegex)
check: compile_json
    clang-tidy -p . $(git ls-files 'src/*.c')

fix: compile_json
    clang-tidy -p . --fix $(git ls-files 'src/*.c')

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
    rm -rf build_test

build_test:
    cmake -B build_test -DCMAKE_BUILD_TYPE=Release -DGB_TEST=ON
    cmake --build build_test --target gb

test path num time: build_test
    timeout --signal=SIGINT {{ time }} ./build_test/gb '{{ path }}' || true
    uv run tests/gameboy-doctor/gameboy-doctor ./logs/gameboy_doctor.log cpu_instrs {{ num }}

test01: (test './tests/gb-test-roms/cpu_instrs/individual/01-special.gb' '1' '2') # passed
test02: (test './tests/gb-test-roms/cpu_instrs/individual/02-interrupts.gb' '2' '2') # passed
test03: (test './tests/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb' '3' '2') # passed
test04: (test './tests/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb' '4' '2') # passed
test05: (test './tests/gb-test-roms/cpu_instrs/individual/05-op rp.gb' '5' '2') # passed
test06: (test './tests/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb' '6' '2') # passed
test07: (test './tests/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb' '7' '4') # passed
test08: (test './tests/gb-test-roms/cpu_instrs/individual/08-misc instrs.gb' '8' '2') # passed
test09: (test './tests/gb-test-roms/cpu_instrs/individual/09-op r,r.gb' '9' '6') # passed
test10: (test './tests/gb-test-roms/cpu_instrs/individual/10-bit ops.gb' '10' '7') # passed
test11: (test './tests/gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb' '11' '10') # passed

test_all: test01 test02 test03 test04 test05 test06 test07 test08 test09 test10 test11

build_render:
    cmake -B build_test -DCMAKE_BUILD_TYPE=Release -DGB_RENDER_TEST=ON
    cmake --build build_test --target gb

test_render: build_render
    [ -f dmg-acid2.gb ] || curl -L -o dmg-acid2.gb https://github.com/mattcurrie/dmg-acid2/releases/download/v1.0/dmg-acid2.gb
    ./build/gb ./dmg-acid2.gb
