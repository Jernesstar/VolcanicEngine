FROM mcr.microsoft.com/devcontainers/cpp:debian

RUN apt-get update && apt-get install -y xorg-dev x11vnc
RUN export DEBIAN_FRONTEND=noninteractive \
    && curl -sSL https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb -o /tmp/chrome.deb \
    && apt-get -y install /tmp/chrome.deb
