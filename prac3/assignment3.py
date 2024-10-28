import socket
import threading
import selectors
import argparse
import sys

HOST = 'localhost'

shared_data = []
shared_data_lock = threading.Lock()

sel = selectors.DefaultSelector()

class Node:
    def __init__(self, data):
        self.data = data
        self.next = None
        self.book_next = None

class LinkedList:
    def __init__(self):
        self.head = None
        self.tail = None
        self.book_heads = {}
        
    def append(self, data, book):
        new_node = Node(data)
        
        if self.tail:
            self.tail.next = new_node
        else:
            self.head = new_node
        self.tail = new_node
        
        print(f'Added data: {data}')
        
        if book not in self.book_heads:
            self.book_heads[book] = new_node
        else:
            current = self.book_heads[book]
            while current.book_next:
                current = current.book_next
            current.book_next = new_node
        
    def print_book(self, book):
        with shared_data_lock:
            if book not in self.book_heads:
                return 'Book not found'
            
            current = self.book_heads[book]
            book_data = []
            
            while current:
                book_data.append(current.data)
                current = current.book_next
            
            print(f'Book: {book}')
            for data in book_data:
                print(data)
                
    def save_to_file(self, book, filename):
        with shared_data_lock:
            if book not in self.book_heads:
                return 'Book not found'
            
            current = self.book_heads[book]
            with open(filename, 'w') as f:
                while current:
                    f.write(current.data + '\n')
                    current = current.book_next
            print(f'Book {book} saved to {filename}')
                      

class NetworkServer:
    def __init__(self, port):
        self.host = HOST
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((HOST, port))
        self.sock.listen(5)
        self.connections_count = 0
        self.lock = threading.Lock()
        self.threads = []
        print(f'Server listening on {HOST}:{port}')
        
        self.linked_list = LinkedList()
        self.connections_count = 0
        
    def handle_client(self, conn, addr):
        with shared_data_lock:
            self.connections_count += 1
            order = self.connections_count
        print(f'New connection from {addr} connect as connection number {order}')
        conn.setblocking(False)
        sel.register(conn, selectors.EVENT_READ, data=None)
        
        try:
            first_line = True
            book = None
            
            while True:
                events = sel.select(timeout=1)
                for key, mask in events:
                    if key.fileobj is conn:
                        try:
                            data = conn.recv(1024).decode('utf-8')
                    
                            if data:
                                if first_line:
                                    book = data.strip()
                                    print(f'Received book: {book}')
                                    first_line = False
                                else:
                                    print(f'Received line: {data.strip()}')
                                    with shared_data_lock:
                                        self.linked_list.append(data.strip(), book)
                            else:
                                print(f'Connection closed by {addr}')
                                sel.unregister(conn)
                                conn.close()
                            
                                filename = f'book_{order:02}.txt'
                                self.linked_list.save_to_file(book, filename)
                                return
                        except BlockingIOError:
                            continue
        except ConnectionResetError as e:
            print(f'Connection closed by {addr}')
            sel.unregister(conn)
            conn.close()
            
    def run(self):
        while True:
            conn, addr = self.sock.accept()
            
            client_thread = threading.Thread(target=self.handle_client, args=(conn, addr))
            self.threads.append(client_thread)
            client_thread.start()
            print(f'Active threads: {threading.active_count() - 1}')
    
    def stop(self):
        print("\nShutting down server...")
        self.sock.close()
        for thread in self.threads:
            thread.join()
        print("Server shut down.")
        
    def signal_handler(self, sig, frame):
        self.stop()
        sys.exit(0)
            

if __name__ == '__main__':
    server = NetworkServer()
    server.run()
            

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-l', dest="port", type=int, default=12345)
    parser.add_argument('-p', '--pattern', type=str)
    args = parser.parse_args()
    listen_port = args.port
    
    if listen_port < 1024:
        print('Port number must under 1024')
        sys.exit(1)
        
    server = NetworkServer(listen_port)
    
    try:
        server.run()
    except KeyboardInterrupt:
        server.stop()
        sys.exit(0)