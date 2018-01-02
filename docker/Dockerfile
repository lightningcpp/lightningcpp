# Dockerfile to install sample web server
FROM scratch

ARG LIGHTNING_TAG_VERSION=master

ADD .build/lightning_$LIGHTNING_TAG_VERSION.tar.gz /

EXPOSE 8888

CMD ["/usr/local/bin/httpserver","/usr/local/share/docs","/usr/local/share/files"]
