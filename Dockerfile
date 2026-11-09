FROM gcc:latest AS builder
WORKDIR /app
COPY . .
RUN gcc -O2 -std=c99 -DMG_ENABLE_HTTP=1 main.c mongoose.c -o server -lpthread -ldl

FROM debian:bookworm-slim
WORKDIR /app
COPY --from=builder /app/server .
EXPOSE 3000
CMD ["./server"]