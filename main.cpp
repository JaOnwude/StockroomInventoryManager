#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

/** Signals that standard input was closed (for example with Ctrl+Z or Ctrl+D). */
struct InputClosed {};

/** Stops waiting for input when the input stream has ended, instead of looping forever. */
void throwIfInputClosed() {
    if (std::cin.eof()) throw InputClosed{};
}

/** Represents one product stored in the stockroom. */
class Product {
public:
    int id{};
    std::string name;
    int quantity{};
    double unitPrice{};

    /** Creates a product from the supplied inventory fields. */
    Product(int productId, std::string productName, int productQuantity, double price)
        : id(productId), name(std::move(productName)), quantity(productQuantity), unitPrice(price) {}
};

/** Manages products and persistence for the stockroom inventory. */
class InventoryManager {
private:
    std::vector<Product> products;
    const std::string fileName = "inventory.txt";

    /** Finds a product by its unique ID; returns nullptr when it is absent. */
    Product* findById(int id) {
        for (Product& product : products) {
            if (product.id == id) return &product;
        }
        return nullptr;
    }

    /** Reads an integer and rejects invalid or out-of-range input. */
    static int readInteger(const std::string& prompt, int minimum) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value && value >= minimum) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            throwIfInputClosed();
            std::cout << "Enter a whole number of at least " << minimum << ".\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    /** Reads a non-empty product name, including spaces. */
    static std::string readName() {
        std::string name;
        while (name.empty()) {
            std::cout << "Product name: ";
            std::getline(std::cin, name);
            throwIfInputClosed();
            if (!name.empty() && name.back() == '\r') name.pop_back();
            if (name.find_first_not_of(" \t") == std::string::npos) name.clear();
            if (name.empty()) std::cout << "Name cannot be empty.\n";
        }
        return name;
    }

    /** Reads and validates a non-negative product price. */
    static double readPrice() {
        double price;
        while (true) {
            std::cout << "Unit price: $";
            if (std::cin >> price && price >= 0.0) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return price;
            }
            throwIfInputClosed();
            std::cout << "Enter a valid non-negative price.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

public:
    /** Loads saved products when the manager starts. */
    void load() {
        std::ifstream input(fileName);
        if (!input) {
            std::cout << "No saved inventory found; starting with an empty stockroom.\n";
            return;
        }
        int id, quantity;
        double price;
        std::string name;
        while (input >> id >> std::quoted(name) >> quantity >> price) {
            products.emplace_back(id, name, quantity, price);
        }
        if (!input.eof()) std::cout << "Some inventory data could not be read.\n";
        std::cout << products.size() << " product(s) loaded.\n";
    }

    /** Writes the current inventory to the local data file. */
    bool save() const {
        std::ofstream output(fileName);
        if (!output) {
            std::cout << "Could not open the inventory file for saving.\n";
            return false;
        }
        for (const Product& product : products) {
            output << product.id << ' ' << std::quoted(product.name) << ' '
                   << product.quantity << ' ' << std::fixed << std::setprecision(2)
                   << product.unitPrice << '\n';
        }
        return static_cast<bool>(output);
    }

    /** Adds a uniquely numbered product and saves the updated inventory. */
    void addProduct() {
        const int id = readInteger("Product ID: ", 1);
        if (findById(id) != nullptr) {
            std::cout << "That ID is already in use.\n";
            return;
        }
        std::string name = readName();
        const int quantity = readInteger("Quantity: ", 0);
        const double price = readPrice();
        products.emplace_back(id, name, quantity, price);
        if (save()) std::cout << "Task complete: product added and inventory saved.\n";
    }

    /** Prints all products in a formatted table or an empty message. */
    void listProducts() const {
        if (products.empty()) {
            std::cout << "The stockroom is empty.\n";
            return;
        }
        std::cout << "\n" << std::left << std::setw(8) << "ID" << std::setw(24) << "Product"
                  << std::right << std::setw(10) << "Quantity" << std::setw(14) << "Unit price" << '\n';
        std::cout << std::string(56, '-') << '\n';
        for (const Product& product : products) {
            std::cout << std::left << std::setw(8) << product.id << std::setw(24) << product.name
                      << std::right << std::setw(10) << product.quantity << "$"
                      << std::fixed << std::setprecision(2) << std::setw(12) << product.unitPrice << '\n';
        }
    }

    /** Searches for and displays one product by ID. */
    void searchProduct() const {
        const int id = readInteger("Enter product ID to find: ", 1);
        for (const Product& product : products) {
            if (product.id == id) {
                std::cout << product.name << " - quantity: " << product.quantity
                          << ", unit price: $" << std::fixed << std::setprecision(2) << product.unitPrice << '\n';
                return;
            }
        }
        std::cout << "No product has ID " << id << ".\n";
    }

    /** Updates the quantity of an existing product. */
    void updateQuantity() {
        const int id = readInteger("Enter product ID to update: ", 1);
        Product* product = findById(id);
        if (product == nullptr) {
            std::cout << "Product not found.\n";
            return;
        }
        product->quantity = readInteger("New quantity: ", 0);
        if (save()) std::cout << "Quantity updated and inventory saved.\n";
    }

    /** Removes an existing product after confirmation. */
    void removeProduct() {
        const int id = readInteger("Enter product ID to remove: ", 1);
        for (auto current = products.begin(); current != products.end(); ++current) {
            if (current->id == id) {
                std::cout << "Remove " << current->name << "? (y/n): ";
                char answer = 'n';
                std::cin >> answer;
                throwIfInputClosed();
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (answer == 'y' || answer == 'Y') {
                    products.erase(current);
                    if (save()) std::cout << "Product removed and inventory saved.\n";
                } else {
                    std::cout << "Removal cancelled.\n";
                }
                return;
            }
        }
        std::cout << "Product not found.\n";
    }

    /** Displays total units and the combined value of all stock. */
    void showSummary() const {
        int totalUnits = 0;
        double totalValue = 0.0;
        for (const Product& product : products) {
            totalUnits += product.quantity;
            totalValue += product.quantity * product.unitPrice;
        }
        std::cout << "Products: " << products.size() << "\nTotal units: " << totalUnits
                  << "\nInventory value: $" << std::fixed << std::setprecision(2) << totalValue << '\n';
    }

    /** Returns the number of products currently in memory. */
    std::size_t size() const { return products.size(); }
};

/** Displays the main menu and returns the user's selected option. */
int showMenu() {
    std::cout << "\n=== Stockroom Inventory Manager ===\n"
              << "1. Add product\n2. List products\n3. Search product\n"
              << "4. Update quantity\n5. Remove product\n6. Inventory summary\n0. Save and exit\n";
    int option;
    while (true) {
        std::cout << "Choose an option: ";
        if (std::cin >> option) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return option;
        }
        throwIfInputClosed();
        std::cout << "Enter a menu number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

/** Runs the inventory menu until the user chooses to save and exit. */
int main() {
    InventoryManager inventory;
    inventory.load();
    bool running = true;
    try {
        while (running) {
            switch (showMenu()) {
                case 1: inventory.addProduct(); break;
                case 2: inventory.listProducts(); break;
                case 3: inventory.searchProduct(); break;
                case 4: inventory.updateQuantity(); break;
                case 5: inventory.removeProduct(); break;
                case 6: inventory.showSummary(); break;
                case 0:
                    running = false;
                    break;
                default: std::cout << "Choose one of the listed options.\n";
            }
        }
    } catch (const InputClosed&) {
        std::cout << "\nInput ended.\n";
    }
    if (!inventory.save()) return 1;
    std::cout << "Inventory saved. Goodbye!\n";
    return 0;
}

