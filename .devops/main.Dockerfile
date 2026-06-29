FROM ubuntu:22.04 AS build
WORKDIR /app

# archive.ubuntu.com flakes intermittently from CI runners; retries +
# longer timeouts let transient connection failures resolve themselves
# rather than failing the whole build.
RUN printf 'Acquire::Retries "5";\nAcquire::http::Timeout "30";\nAcquire::https::Timeout "30";\n' \
      > /etc/apt/apt.conf.d/80-retries && \
  apt-get update && \
  apt-get install -y --fix-missing build-essential wget cmake git ninja-build \
  && rm -rf /var/lib/apt/lists/* /var/cache/apt/archives/*

COPY . .
ARG STELNET_BUILD_JOBS
RUN jobs="${STELNET_BUILD_JOBS:-$(nproc)}" && \
  cmake -S . -B build -G Ninja -DSTELNET_BUILD_TESTS=OFF && \
  cmake --build build -j"${jobs}" --target stelnet-cli

FROM ubuntu:22.04 AS runtime
WORKDIR /app

RUN printf 'Acquire::Retries "5";\nAcquire::http::Timeout "30";\nAcquire::https::Timeout "30";\n' \
      > /etc/apt/apt.conf.d/80-retries && \
  apt-get update && \
  apt-get install -y --fix-missing curl ffmpeg libsdl2-dev wget cmake git tini \
  && rm -rf /var/lib/apt/lists/* /var/cache/apt/archives/*

COPY --from=build /app /app
RUN useradd -m -u 1000 stelnet && \
  mkdir -p /cache /models && \
  chown -R stelnet:stelnet /app /cache /models
ENV PATH=/app/build/bin:$PATH
ENV STELNET_CACHE_DIR=/cache
USER stelnet
ENTRYPOINT [ "tini", "--", "bash", "/app/.devops/run-server.sh" ]
