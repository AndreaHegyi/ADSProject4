#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
using namespace std;

class BST
{
public:
    static int LineCtr;
    string key, softName, softVer;
    int qty, price;
    int fileLoc;
    BST *left, *right;
    BST() : key(""), softName(""), softVer(""), qty(0), price(0), fileLoc(-1), left(NULL), right(NULL) {}
    BST(string _softName, string _softVer, int _qty, int _price, int _fileLoc)
    {
        key = _softName + _softVer;
        softName = _softName;
        softVer = _softVer;
        qty = _qty;
        price = _price;
        fileLoc = _fileLoc;
        left = right = NULL;
    }
    BST *insert(BST *root, string _softName, string _softVer, int _qty, int _price, int _fileLoc)
    {
        BST *searchNode = search(root, _softName + _softVer);
        if (searchNode)
        {
            searchNode->qty += _qty;
            return root;
        }
        if (!root)
        {
            LineCtr++;
            return new BST(_softName, _softVer, _qty, _price, _fileLoc);
        }
        if (_softName + _softVer > root->key)
            root->right = insert(root->right, _softName, _softVer, _qty, _price, _fileLoc);
        else
            root->left = insert(root->left, _softName, _softVer, _qty, _price, _fileLoc);
        return root;
    }
    void inorder(BST *root)
    {
        if (root)
        {
            inorder(root->left);
            cout << "--> " << root->softName << "  " << root->softVer << "  " << root->qty << " " << root->price << endl;
            inorder(root->right);
        }
    }
    BST *minValueNode(BST *node)
    {
        BST *current = node;
        while (current && current->left != NULL)
            current = current->left;
        return current;
    }
    BST *deleteNode(BST *root, string key)
    {
        if (root == NULL)
            return root;
        if (key < root->key)
            root->left = deleteNode(root->left, key);
        else if (key > root->key)
            root->right = deleteNode(root->right, key);
        else
        {
            if (root->left == NULL)
            {
                BST *temp = root->right;
                delete(root);
                return temp;
            }
            else if (root->right == NULL)
            {
                BST *temp = root->left;
                delete(root);
                return temp;
            }
            BST *temp = minValueNode(root->right);
            root->key = temp->key;
            root->softName = temp->softName;
            root->softVer = temp->softVer;
            root->qty = temp->qty;
            root->price = temp->price;
            root->right = deleteNode(root->right, temp->key);
        }
        return root;
    }
    BST *search(BST *root, string key)
    {
        if (root == NULL || root->key == key)
            return root;

        if (root->key < key)
            return search(root->right, key);
        return search(root->left, key);
    }
    BST *searchByFileLoc(BST *root, int loc)
    {
        if (root == NULL || root->fileLoc == loc)
            return root;

        BST *res = searchByFileLoc(root->right, loc);

        if (!res)
            return searchByFileLoc(root->left, loc);
        else
            return res;
    }
};
int BST::LineCtr = 0;
class SoftwareStore
{
private:
    BST b, *root = NULL;
    bool isEmpty = true;
    string fileName;

public:
    ~SoftwareStore()
    {
        dumpChangesToFile();
    }
    SoftwareStore(string fn)
    {
        fileName = fn;
        fstream dataStream;
        dataStream.open(fileName, fstream::in);
        char temp[100];
        while (!dataStream.eof())
        {
            string name, version;
            int qty, price = -1;
            dataStream.getline(temp, 100);
            if (!strlen(temp))
                return;
            char *ptr = strtok(temp, ",");
            if (!ptr)
                return;
            name = ptr;
            ptr = strtok(nullptr, ",");
            if (!ptr)
                return;
            version = ptr;
            ptr = strtok(nullptr, ",");
            if (!ptr)
                return;
            qty = atoi(ptr);
            ptr = strtok(nullptr, ",");
            if (!ptr)
                return;
            price = atoi(ptr);
            if (!root)
                root = b.insert(root, name, version, qty, price, b.LineCtr);
            else
                b.insert(root, name, version, qty, price, b.LineCtr);
            delete ptr;
            isEmpty = false;
        }
        dataStream.close();
    }

    void appendSoftware(string name, string version, int qty, int price)
    {
        BST *foundNode = b.search(root, name + version);
        if (foundNode)
        {
            foundNode->qty += qty;
            return;
        }
        if (!root)
            root = b.insert(root, name, version, qty, price, b.LineCtr);
        else
            b.insert(root, name, version, qty, price, b.LineCtr);
        cout << "[OUTPUT] Operation completed!" << endl;
    }

    void sellSoftware(string name, string version, int qty)
    {
        BST *foundNode = b.search(root, name + version);
        if (foundNode)
        {
            if (foundNode->qty - qty > 0)
            {
                foundNode->qty -= qty;
                cout << "[OUTPUT] Operation completed!" << endl;
            }
            else if (foundNode->qty - qty == 0)
            {
                root = b.deleteNode(root, name + version);
                cout << "[OUTPUT] Operation completed! Warning: Software sold out!" << endl;
            }
            else
                cout << "[OUTPUT] Insufficient available software of this kind!" << endl;
        }
        else
            cout << "[OUTPUT] Software doesn't exist in the store!" << endl;
    }

    void dumpChangesToFile()
    {

        fstream dataStream;
        dataStream.open(fileName.c_str(), fstream::out);
        int currLine = 0;
        BST *currNode;
        while (currLine < b.LineCtr)
        {
            currNode = b.searchByFileLoc(root, currLine);
            if (currNode)
                dataStream << currNode->softName << ',' << currNode->softVer << ',' << currNode->qty << "," << currNode->price << endl;
            currLine++;
        }
        dataStream.close();
        cout << "[OUTPUT] All changes dumped to file!" << endl;
    }

    void findSoftware(string name, string version)
    {
        BST *foundNode = b.search(root, name + version);
        if (foundNode)
            cout << "[OUTPUT] Software found! Quantity: " << foundNode->qty << " Price: " << foundNode->price << endl;
        else
            cout << "[OUTPUT] Software not found!" << endl;
    }
    void printAll()
    {
        if (!root)
        {
            cout << "[OUTPUT] No software currently in store!" << endl;
            return;
        }
        cout << "[OUTPUT] Software in the store:" << endl;
        b.inorder(root);
        cout << endl;
    }
};

SoftwareStore s("software.txt");
void appendSoftware()
{
    cin.ignore();
    string name, version;
    int qty, price;
    cout << "[OUTPUT] Name: ";
    getline(cin, name);
    cout << "[OUTPUT] Version: ";
    cin >> version;
    cout << "[OUTPUT] Quantity: ";
    cin >> qty;
    cout << "[OUTPUT] Price: ";
    cin >> price;
    s.appendSoftware(name, version, qty, price);
    cout << endl;
}
void sellSoftware()
{
    cin.ignore();
    string name, version;
    int qty;
    cout << "[OUTPUT] Software's name: ";
    getline(cin, name);
    cout << "[OUTPUT] Software's version: ";
    cin >> version;
    cout << "[OUTPUT] Quantity to sell: ";
    cin >> qty;
    s.sellSoftware(name, version, qty);
    cout << endl;
}
void findSoftware()
{
    cin.ignore();
    string name, version;
    cout << "[OUTPUT] Software's name: ";
    getline(cin, name);
    cout << "[OUTPUT] Software's version: ";
    getline(cin, version);
    s.findSoftware(name, version);
    cout << endl;
}
void dumpChangesToFile()
{
    s.dumpChangesToFile();
    cout << endl;
}
void printAll()
{
    s.printAll();
    cout << endl;
}

int main()
{
    int opt = -1;
    do
    {
        cout << "...........SOFTWARE STORE PROGRAM............." << endl;
        cout << "[1] Append/Update new software to the store" << endl;
        cout << "[2] Sell software" << endl;
        cout << "[3] Find software in store" << endl;
        cout << "[4] Dump changes to file right now" << endl;
        cout << "[5] Print software from BST (inorder)" << endl;
        cout << "[6] Exit program" << endl;
        cout << ".............................................." << endl;
        cout << "Your command: ";
        cin >> opt;
        switch (opt)
        {
            case 1:
                appendSoftware();
                break;
            case 2:
                sellSoftware();
                break;
            case 3:
                findSoftware();
                break;
            case 4:
                dumpChangesToFile();
                break;
            case 5:
                printAll();
                break;
            case 6:
                return 0;
            default:
                cout << "Command unrecognized" << endl;
                return 0;
        }

    } while (opt != 6);
    return 0;
}
