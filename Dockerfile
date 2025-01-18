# Build stage
FROM stateoftheartio/qt6:6.8-gcc-aqt AS builder

# Install additional dependencies including Qt Creator and FFTW
RUN sudo apt-get update && sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    libgl-dev \
    libvulkan-dev \
    libeigen3-dev \
    libboost-all-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libx11-xcb-dev \
    libxcb-*-dev \
    libcups2-dev \
    qtcreator \
    libfftw3-dev \
    libfftw3-3 \
    libfftw3-bin \
    && sudo rm -rf /var/lib/apt/lists/* \
    && sudo ldconfig  # Update the dynamic linker run-time bindings

# Create and set proper permissions for project directory and Qt Creator configs
USER root
RUN mkdir -p /home/user/project && \
    mkdir -p /home/user/.config/QtProject/qtcreator && \
    mkdir -p /home/user/.cache/QtProject && \
    chown -R user:user /home/user/project && \
    chown -R user:user /home/user/.config && \
    chown -R user:user /home/user/.cache && \
    chmod -R 755 /home/user/.config && \
    chmod -R 755 /home/user/.cache
USER user

WORKDIR /home/user/project
COPY --chown=user:user . .

# Fix UI file since we're using Qt 6.8
RUN cp src/View/MainWindow/mainwindow.ui src/View/MainWindow/mainwindow.ui.bak && \
    sed -i '/QPalette::Accent/d' src/View/MainWindow/mainwindow.ui

# Build the project
RUN qt-cmake . -G Ninja -B ./build && \
    cmake --build ./build

# Runtime stage
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV QT_QPA_PLATFORM=xcb
ENV QT_X11_NO_MITSHM=1
ENV XDG_RUNTIME_DIR=/tmp/runtime-root
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

# Install runtime dependencies
RUN apt-get update && \
    apt-get install -y \
    libqt6core6 \
    libqt6gui6 \
    libqt6widgets6 \
    libqt6printsupport6 \
    libgl1-mesa-glx \
    libglu1-mesa \
    libxkbcommon0 \
    libxkbcommon-x11-0 \
    libvulkan1 \
    libegl1 \
    libeigen3-dev \
    libboost-dev \
    libxcb1 \
    libx11-6 \
    libxcb-*0 \
    mesa-utils \
    xvfb \
    libcups2 \
    libfftw3-3 \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir -p /tmp/runtime-root \
    && chmod 700 /tmp/runtime-root \
    && ldconfig  # Update the dynamic linker run-time bindings

WORKDIR /app
COPY --from=builder /home/user/project/build/ECGProcessing /app/
COPY pliki_testowe /app/pliki_testowe

CMD ["./ECGProcessing"]