
# Project Name

## Prerequisites

Before running this project, ensure you have the following:

- **liburcu**: Clone and build the [liburcu repository](link-to-liburcu-repo) according to its installation instructions.

## How to Run

To build and run the code, follow these steps:

1. **Clone the Repository**
   
   ```bash
   git clone https://github.com/your-username/your-repo.git
   ```
   
2. **Navigate to the Respective Folders**

```bash
cd folder-name
```
3. **Build the Code**

Run the following command to compile the code:

```bash
g++ -o run main.cpp b+tree.cpp lock.h lock.c -lpthread -g
```
4. **Run the Application**

After successfully building the code, execute the generated executable:

```bash

./run 1000 100 10000 100 64
```
