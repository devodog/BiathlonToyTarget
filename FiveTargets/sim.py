import serial # type: ignore

def main():
    # Configure the COM port and baud rate
    com_port = 'COM13'  # Change this to your COM port
    baud_rate = 9600    # Change this to your desired baud rate

    # Open the serial port
    try:
        ser = serial.Serial(com_port, baud_rate)
        print(f"Connected to {com_port} at {baud_rate} baud.")
    except serial.SerialException as e:
        print(f"Error opening COM port: {e}")
        return

    # Open the text file
    try:
        with open('testdata.txt', 'r') as file:
            lines = file.readlines()
    except FileNotFoundError:
        print("Error: testdata.txt not found.")
        return

    line_index = 0

    while True:
        command = input("Enter command (send/exit): ").strip().lower()

        if command == "send":
            if line_index < len(lines):
                line_to_send = lines[line_index].strip()  # Read the next line
                ser.write(line_to_send.encode('utf-8'))  # Send to COM port
                ser.write(('\r\n').encode('utf-8'))  # Sending carrage return and line feed to COM port
                print(f"Sent: {line_to_send}")
                line_index += 1
            else:
                print("End of file reached. No more lines to send.")
                break  # Exit the loop if end of file is reached

        elif command == "exit":
            print("Exiting the program.")
            break

        else:
            print("Invalid command. Please enter 'send' or 'exit'.")

    # Clean up
    ser.close()
    print("COM port closed.")

if __name__ == "__main__":
    main()
