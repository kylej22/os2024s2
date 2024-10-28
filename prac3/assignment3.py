import socket
import threading
import selectors
import argparse
import sys
import signal
import time

HOST = 'localhost'

shared_data_lock = threading.Lock()
analysis_lock = threading.Lock()

sel = selectors.DefaultSelector()

class Node:
    """Node class for linked list to store data and references to next nodes."""

    def __init__(self, data):
        """
        Initialize a new node.

        Args:
            data: The data to store in the node.
        """
        self.data = data
        self.next = None
        self.book_next = None

class LinkedList:
    """Linked list to manage a collection of notes categorized by book titles."""

    def __init__(self):
        """Initialize an empty linked list with a dictionary for book heads."""
        self.head = None
        self.tail = None
        self.book_heads = {}
        
    def append(self, data, book):
        """
        Append a new data entry to the linked list under a specified book.

        Args:
            data: The data (note) to append.
            book: The title of the book associated with the data.
        """
        new_node = Node(data)
        
        if self.tail:
            self.tail.next = new_node
        else:
            self.head = new_node
        self.tail = new_node
        
        if data != "":
            print(f'Added data: {data}')
        
        if book not in self.book_heads:
            self.book_heads[book] = new_node
        else:
            current = self.book_heads[book]
            while current.book_next:
                current = current.book_next
            current.book_next = new_node
            
    def get_books_containing_pattern(self, pattern):
        """
        Get a dictionary of book titles and their corresponding occurrence counts of a specific pattern.

        Args:
            pattern: The search pattern to look for in the notes.

        Returns:
            A dictionary with book titles as keys and occurrence counts as values.
        """
        with shared_data_lock:
            frequency = {}
            for book, node in self.book_heads.items():
                count = 0
                current = node
                while current:
                    if pattern in current.data:
                        count += 1
                    current = current.book_next
                frequency[book] = count
            return frequency
                      

class NetworkServer:
    """Server class to handle client connections and data analysis for search patterns."""

    def __init__(self, port, pattern):
        """
        Initialize the server to listen for incoming client connections.

        Args:
            port: The port on which the server will listen.
            pattern: The search pattern to analyze in the received data.
        """
        self.host = HOST
        self.port = port
        self.pattern = pattern
        self.interval = 5  
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((HOST, port))
        self.sock.listen(5)
        self.connections_count = 0
        self.lock = threading.Lock()
        self.threads = []
        print(f'Server listening on {HOST}:{port}')
        
        self.linked_list = LinkedList()
        
        self.start_analysis_threads()
        
    def handle_client(self, conn, addr):
        """
        Handle communication with a connected client.

        Args:
            conn: The socket connection to the client.
            addr: The address of the connected client.
        """
        with self.lock:
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
                                    first_line = False
                                else:
                                    with shared_data_lock:
                                        self.linked_list.append(data.strip(), book)
                            else:
                                print(f'Connection closed by {addr}')
                                sel.unregister(conn)
                                conn.close()
                                return
                        except BlockingIOError:
                            continue
        except ConnectionResetError as e:
            print(f'Connection closed by {addr} due to an error: {e}')
            sel.unregister(conn)
            conn.close()
            
    def run(self):
        """Run the server to accept incoming client connections."""
        while True:
            conn, addr = self.sock.accept()
            client_thread = threading.Thread(target=self.handle_client, args=(conn, addr))
            self.threads.append(client_thread)
            client_thread.start()
            print(f'Active threads: {threading.active_count() - 1}')
    
    def start_analysis_threads(self):
        """Start analysis threads to periodically analyze data for the search pattern."""
        for _ in range(2):
            analysis_thread = threading.Thread(target=self.analysis_worker)
            self.threads.append(analysis_thread)
            analysis_thread.start()
    
    def analysis_worker(self):
        """Perform analysis on the data collected and output frequency of the search pattern."""
        while True:
            time.sleep(self.interval)
            with analysis_lock:
                frequencies = self.linked_list.get_books_containing_pattern(self.pattern)
                sorted_books = sorted(frequencies.items(), key=lambda x: x[1], reverse=True)
                
                print(f'\nPattern Analysis ({self.pattern}):')
                for book, count in sorted_books:
                    if count > 0:
                        print(f'{book}: {count} occurrences')
                print()
    
    def stop(self):
        """Stop the server and join all threads."""
        print("\nShutting down server...")
        self.sock.close()
        for thread in self.threads:
            thread.join()
        print("Server shut down.")
        
    def signal_handler(self, sig, frame):
        """Handle termination signals to gracefully shut down the server.

        Args:
            sig: The signal number.
            frame: The current stack frame.
        """
        self.stop()
        sys.exit(0)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-l', dest="port", type=int, default=12345)
    parser.add_argument('-p', '--pattern', type=str, required=True)
    args = parser.parse_args()
    
    listen_port = args.port
    search_pattern = args.pattern
    
    if listen_port < 1024:
        print('Port number must be above 1024')
        sys.exit(1)
        
    server = NetworkServer(listen_port, search_pattern)
    signal.signal(signal.SIGINT, server.signal_handler)
    
    try:
        server.run()
    except KeyboardInterrupt:
        server.stop()
        sys.exit(0)
