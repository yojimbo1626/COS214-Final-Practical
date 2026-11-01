FROM ubuntu:24.04

# Compiler, CMake and git (CMake downloads doctest and cpp-httplib while configuring)
RUN apt-get update \
 && apt-get install -y --no-install-recommends g++ cmake make git ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Configure and build all targets: engine, unit_tests, tividy_gui
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
 && cmake --build build -j"$(nproc)"

# The image only builds if every test passes
RUN ctest --test-dir build --output-on-failure

EXPOSE 8080
CMD ["./build/tividy_gui", "8080", "gui/web"]
