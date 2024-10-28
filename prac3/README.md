# Assignment 3

## Usage

Start the server with:

```bash
python3 assignment3.py -l <port> -p "<pattern>"
```

Example:

```bash
python3 assignment3.py -l 12345 -p "hello"
```

## Testing:

**test.sh** provides a script to simulate multiple clients connecting to the server

To manually connect use:

```bash
nc localhost 12345 -q 0 < book.txt
```
