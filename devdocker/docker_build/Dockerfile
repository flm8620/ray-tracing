FROM nvidia/cuda:12.4.1-cudnn-devel-ubuntu22.04

# Set environment variables to non-interactive (This prevents some prompts)
ENV DEBIAN_FRONTEND=noninteractive

# Update and install packages
RUN apt update && apt install -y \
    sudo \
    lsb-release \
    software-properties-common \
    curl \
    wget \
    vim \
    htop \
    git \
    cmake \
    cmake-curses-gui

RUN apt install -y \
    build-essential \
    libboost-all-dev \
    libeigen3-dev \
    libgoogle-glog-dev \
    libgtest-dev \
    libglew-dev \
    qtbase5-dev \
    libqt5opengl5-dev
    

ARG USER_ID
ARG GROUP_ID
ARG USER_NAME

# Create a new group and user with the passed user name, then add them to the 'sudo' group
RUN groupadd -g ${GROUP_ID} ${USER_NAME} && \
    useradd -m -u ${USER_ID} -g ${USER_NAME} -s /bin/bash ${USER_NAME} && \
    mkdir -p /etc/sudoers.d/ && \
    echo "${USER_NAME} ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/${USER_NAME}

# Change to non-root privilege
USER ${USER_NAME}

WORKDIR /home/${USER_NAME}

COPY install_opencv4.sh pkg_source/opencv-4.10.0.tar.gz /home/${USER_NAME}/

RUN bash /home/${USER_NAME}/install_opencv4.sh

# Set the default command for the container
CMD ["bash"]