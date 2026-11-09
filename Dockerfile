FROM gcc:latest AS builder
WORKDIR /app
COPY . .

# Print files to verify mongoose.c and main.c are present in the container
RUN ls -la

# Compile with verbose error output
RUN gcc -O2 -std=gnu99 -DMG_ENABLE_HTTP=1 main.c mongoose.c -o server -lpthread -ldl

FROM debian:bookworm-slim
WORKDIR /app
COPY --from=builder /app/server .
EXPOSE 3000
CMD ["./server"]