# Base image: latest Ubuntu 24.04 LTS slim
FROM ubuntu:24.04

# --- Arguments ---
ARG USERNAME=user
ARG UID=1000
ARG GID=1000
ARG WGET_ARGS="-q --show-progress --progress=bar:force:noscroll"
ARG PYTHON_VENV_PATH=/opt/zephyr-venv
ARG ZSDK_VERSION=0.17.4
ARG HOSTTYPE=x86_64

# --- Environment variables ---
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8
ENV ZEPHYR_BASE=/opt/zephyrproject/zephyr
ENV PATH="$ZEPHYR_BASE:$PATH:/usr/bin"
ENV ZSDK_VERSION=$ZSDK_VERSION
ENV HOSTTYPE=${HOSTTYPE}
ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
ENV ZEPHYR_SDK_INSTALL_DIR=/opt/toolchains/zephyr-sdk-${ZSDK_VERSION}
ENV PATH="${ZEPHYR_SDK_INSTALL_DIR}/sysroots/x86_64-pokysdk-linux/usr/bin:$PATH"
ENV PATH="${PYTHON_VENV_PATH}/bin:$PATH"

# --- Install essential build tools ---
RUN apt-get update && apt-get upgrade -y && apt-get install -y --no-install-recommends \
        build-essential \
        clang \
        cmake \
        curl \
        gdb-multiarch \
        git \
        gcc-arm-none-eabi \
        libudev1 \
        libusb-1.0-0 \
        locales \
        ninja-build \
        openocd \
        python3 \
        python3-pip \
        python3-setuptools \
        python3-venv \
        sudo \
        unzip \
        wget \
    && apt-get autoremove --purge -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# --- Locale ---
RUN locale-gen en_US.UTF-8

# --- Install west ---
RUN python3 -m venv $PYTHON_VENV_PATH \
    && $PYTHON_VENV_PATH/bin/pip install --no-cache-dir west

# --- Create user account ---
RUN <<EOF
    userdel -r ubuntu || true
    groupadd -g $GID -o $USERNAME
    useradd -u $UID -m -g $USERNAME -G plugdev $USERNAME
    echo $USERNAME ' ALL = NOPASSWD: ALL' > /etc/sudoers.d/$USERNAME
    chmod 0440 /etc/sudoers.d/$USERNAME
EOF

USER root

# --- Zephyr SDK install (only ARM and x86_64 toolchains) ---
RUN <<EOF
    mkdir -p /opt/toolchains
    cd /opt/toolchains
    wget ${WGET_ARGS} https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZSDK_VERSION}/zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.xz
    tar xf zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.xz
    zephyr-sdk-${ZSDK_VERSION}/setup.sh -t arm-zephyr-eabi -t x86_64-zephyr-elf -c
    rm zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.xz
EOF

# --- Run Zephyr SDK setup as user for CMake registry ---
USER $USERNAME
RUN sudo -E /bin/bash -c " \
        /opt/toolchains/zephyr-sdk-${ZSDK_VERSION}/setup.sh -c && \
        chown -R $USERNAME:$USERNAME /home/$USERNAME/.cmake \
    "

USER root

# --- Minimal Zephyr base clone (shallow, latest commit) ---
RUN git clone --depth 1 https://github.com/zephyrproject-rtos/zephyr.git $ZEPHYR_BASE

WORKDIR $ZEPHYR_BASE

# --- Notes ---
# This setup supports only:
#   - native_sim (native_posix) board
#   - nrf52840_dongle board
# Other boards will not work unless additional SDK toolchains are installed.
