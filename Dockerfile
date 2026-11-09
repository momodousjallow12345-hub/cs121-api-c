FROM gcc:latest AS builder
WORKDIR /app
COPY . .
RUN gcc -std=c99 -D_GNU_SOURCE -DMG_ENABLE_HTTP=1 main.c mongoose.c -o server -lpthread -ldl

FROM debian:bookworm-slim
WORKDIR /app
COPY --from=builder /app/server .
EXPOSE 3000
CMD ["./server"]