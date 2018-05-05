#include <list>
#include <iostream>
#include <vector>

using namespace std;

class Node
{
public:
    int num;
    int count;
};

class HashTable
{
public:
    typedef list<Node>::iterator ListIter;
private:
    list<Node> m_Container[10];
    int HashFunction(const int& v) const;
    int m_Count;
public:
    HashTable();
    ~HashTable();
    void Insert(const int& v);
    bool Find(const int& v);
    bool Delete(const int& v);    
    int Count() const;
};

HashTable::HashTable()
{
    m_Count = 0;
}

HashTable::~HashTable()
{
    m_Count = 0;
    for (int i = 0; i < 10; i++)
    {
        m_Container[i].clear();
    }
}

int HashTable::HashFunction(const int& v) const
{
    return v%10;
}

int HashTable::Count() const
{
    return m_Count;
}

void HashTable::Insert(const int& v)
{
    int hashRes = HashFunction(v);
    for (ListIter FindIter = m_Container[hashRes].begin(); FindIter != m_Container[hashRes].end(); ++FindIter)
    {
        if ((*FindIter).num == v)
        {
            (*FindIter).num++;
            return;
        }
    }
    m_Container[hashRes].push_back(Node{v, 1});
    ++m_Count;
}

bool HashTable::Find(const int& v)
{
    int hashRes = HashFunction(v);
    for (ListIter FindIter = m_Container[hashRes].begin(); FindIter != m_Container[hashRes].end(); ++FindIter)
    {
        if ((*FindIter).num == v)
        {
            return true;
        }
    }
    return false;
}

bool HashTable::Delete(const int& v)
{
    int hashRes = HashFunction(v);
    for (ListIter DelIter = m_Container[hashRes].begin(); DelIter != m_Container[hashRes].end(); ++DelIter)
    {
        if ((*DelIter).num == v)
        {
            if ((*DelIter).count == 1)
                m_Container[hashRes].erase(DelIter);
            else
                (*DelIter).count--;
            m_Count--;
            return true;
        } 
    }
    return false;
}

int main(int argc, char *argv[])
{
    HashTable source;
    source.Insert(1);
    bool trueFind;
    trueFind = source.Find(1);
    cout << trueFind << endl;

    vector<string> test{"a", "hello", "American", "hello"};
    cout << test[2] << endl;
    return 0;
}