import socket
import threading
import selectors

HOST = 'localhost'
PORT = 12345

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
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((HOST, PORT))
        self.sock.listen()
        print(f'Server listening on {HOST}:{PORT}')
        
        self.linked_list = LinkedList()
        self.connections_count = 0
        
    def handle_client(self, conn, addr):
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
                        data = conn.recv(1024).decode('utf-8')
                    
                        if data:
                            if first_line:
                                book = data
                                print(f'Received book: {book}')
                                first_line = False
                            else:
                                print(f'Received line: {data}')
                                with shared_data_lock:
                                    self.linked_list.append(data, book)
                        else:
                            print(f'Connection closed by {addr}')
                            sel.unregister(conn)
                            conn.close()
                            
                            filename = f'book_{order}.txt'
                            self.linked_list.save_to_file(book, filename)
                            return
        except ConnectionResetError as e:
            print(f'Connection closed by {addr}')
            sel.unregister(conn)
            conn.close()
            
    def run(self):
        while True:
            conn, addr = self.sock.accept()
            
            client_thread = threading.Thread(target=self.handle_client, args=(conn, addr))
            client_thread.start()
            print(f'Active threads: {threading.active_count() - 1}')
            

if __name__ == '__main__':
    server = NetworkServer()
    server.run()