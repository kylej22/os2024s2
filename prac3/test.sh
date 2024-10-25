for i in {1..11}; do
    nc localhost 12345 -i 1 < text-files/book${i}.txt &
done
wait
