# QtLucide Docker Build Environment
# Multi-stage build for development and runtime

# Build stage
FROM ubuntu:22.04 AS builder

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-pip \
    wget \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# Install Qt6
RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    qt6-svg-dev \
    libqt6svg6-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Create build directory and configure
RUN mkdir -p build && cd build && \
    cmake .. \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DQTLUCIDE_BUILD_EXAMPLES=ON \
    -DQTLUCIDE_BUILD_TESTS=ON

# Build the project
RUN cd build && ninja

# Run tests
RUN cd build && ctest --output-on-failure

# Runtime stage (minimal)
FROM ubuntu:22.04 AS runtime

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libqt6core6 \
    libqt6gui6 \
    libqt6widgets6 \
    libqt6svg6 \
    && rm -rf /var/lib/apt/lists/*

# Copy built libraries and examples
COPY --from=builder /app/build/src/libQtLucide.so* /usr/local/lib/
COPY --from=builder /app/build/examples/ /usr/local/bin/examples/
COPY --from=builder /app/include/ /usr/local/include/

# Update library cache
RUN ldconfig

# Set working directory
WORKDIR /usr/local/bin

# Default command
CMD ["./examples/gallery/QtLucide Gallery"]

# Development stage (includes build tools)
FROM builder AS development

# Install additional development tools
RUN apt-get update && apt-get install -y \
    gdb \
    valgrind \
    clang-format \
    clang-tidy \
    cppcheck \
    doxygen \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Default command for development
CMD ["/bin/bash"]
