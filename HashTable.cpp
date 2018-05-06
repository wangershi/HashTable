#include <list>
#include <iostream>
#include <vector>
#include <string.h>
#include <iomanip>

using namespace std;
typedef unsigned long ULONG;
#define HASH_MAX_STRING_LEN 128
#define AlignSize 16

class HashTable 
{
protected:  
    struct Assoc   
    {
        Assoc() 
        {
            count = 0;  
            pNext = NULL;  
        }  
        
        string searchString; 
        int count;  
        Assoc* pNext;
    };
      
    typedef Assoc* LPAssoc;  
public:  
    struct iterator   
    {  
        friend class HashTable;  
        iterator()  
        {  
            m_pIter = NULL;  
            m_nIndex = 0;  
            m_pMap = NULL;  
        } 

    protected:  
        LPAssoc m_pIter;  
        ULONG m_nIndex;  
        HashTable* m_pMap;  
    };  
        
    HashTable(ULONG nInitSize = 199, bool bAutoIncr = true)  
    {
        m_nHashSize = 0;  
        m_nCount = 0;
        m_pHashTable = NULL;  
        InitMap(nInitSize);  
    }  
      
    bool Insert(const string sKey)  
    {  
        if ( sKey.size() <= 0 || sKey.size() > HASH_MAX_STRING_LEN )  
        {  
            return false;  
        }

        ULONG nHash = BkdrHashKey(sKey) % m_nHashSize;  
        LPAssoc pAssoc = m_pHashTable[nHash];  
        if ( NULL == pAssoc )  
        {  
            m_pHashTable[nHash] = new Assoc;
            m_pHashTable[nHash]->searchString = sKey;  
            m_pHashTable[nHash]->count = 1;  
            m_pHashTable[nHash]->pNext = NULL;  
            m_nCount++;  
        }
        else  
        {  
            LPAssoc pAssocPre = pAssoc;  
            while( pAssoc )  
            {  
                // 重复插入同一sKey，count加1，m_nCount不加1
                if ( pAssoc->searchString == sKey)
                {
                    pAssoc->count++;
                    break;
                }
                pAssocPre = pAssoc;  
                pAssoc = pAssoc->pNext;  
            }
            if ( NULL == pAssoc )  
            {
                pAssoc = new Assoc;  
                pAssoc->searchString = sKey;  
                pAssoc->count = 1;  
                pAssoc->pNext = NULL;  
                pAssocPre->pNext = pAssoc;
                m_nCount ++;
            } 
        }  
      
        if ( m_nCount > m_nHashSize ) 
        {  
            ReSetTableSize( AdjustSize(m_nCount) );  
        }  
            
        return true;  
    }  

    bool Find(const string sKey, int& count)  
    {  
        if ( sKey.size() <= 0 || sKey.size() > HASH_MAX_STRING_LEN )  
        {  
            return false;  
        }  
              
        ULONG nHash = BkdrHashKey(sKey);  
        nHash = nHash % m_nHashSize;  
        LPAssoc pAssoc = m_pHashTable[nHash];  
        while( pAssoc )  
        {  
            if ( pAssoc->searchString == sKey)
            {  
                count = pAssoc->count;  
                return true;  
            }  
            pAssoc = pAssoc->pNext;  
        }
        return false;  
    }

    void PrintTable()
    {
        cout << "Begin to print the hash table..." << endl;
        cout << left << setw(AlignSize) << "key" << "| " << right << setw(AlignSize) << "count" << endl; 
        cout << "----------------|-----------------" << endl;
        for ( ULONG i=0; i<m_nHashSize; i++ )
        {
            LPAssoc pAssoc = m_pHashTable[i];
            while( pAssoc )
            {
                cout << left << setw(AlignSize) << pAssoc->searchString  << "| " << right << setw(AlignSize) << pAssoc->count << endl; 
                pAssoc = pAssoc->pNext;
            }
        }
    }

    ULONG GetTableSize()
    {  
        return m_nHashSize;  
    }

    ULONG GetCount()  
    {  
        return m_nCount;  
    }

protected:  
    void ReSetTableSize(ULONG nSize)  
    {
        LPAssoc* pNewAssocTable = new LPAssoc[nSize];
        memset(pNewAssocTable,0,sizeof(LPAssoc)*nSize);  
        for ( ULONG i = 0;i < m_nHashSize;i++ )  
        {
            LPAssoc pOldAssoc = m_pHashTable[i];
            while( pOldAssoc )  
            {
                ULONG nHash = BkdrHashKey(pOldAssoc->searchString)%nSize;
                if ( NULL == pNewAssocTable[nHash] )  
                {
                    pNewAssocTable[nHash] = new Assoc;
                    pNewAssocTable[nHash]->searchString = pOldAssoc->searchString;
                    pNewAssocTable[nHash]->count = pOldAssoc->count;
                    pNewAssocTable[nHash]->pNext = NULL;
                }
                else
                {
                    LPAssoc pAssocTemp = pNewAssocTable[nHash];  
                    while( pAssocTemp->pNext )
                        pAssocTemp = pAssocTemp->pNext;
                    LPAssoc pAssoc = new Assoc;
                    pAssocTemp->pNext = pAssoc;
                    pAssoc->pNext = NULL;

                    pAssoc->searchString = pOldAssoc->searchString;
                    pAssoc->count = pOldAssoc->count;
                }  
                pOldAssoc = pOldAssoc->pNext;  
            }  
        }
      
        delete[] m_pHashTable;  
        m_pHashTable = pNewAssocTable;  
        m_nHashSize = nSize;
    }  
        
    void InitMap(ULONG nSize)  
    {  
        m_nHashSize = AdjustSize(nSize);  
        if ( m_pHashTable )  
        {  
            delete[] m_pHashTable;  
            m_pHashTable = NULL;  
        }  
      
        m_pHashTable = new LPAssoc[m_nHashSize];  
        memset(m_pHashTable,0,sizeof(LPAssoc)*m_nHashSize );  
    }
        
    ULONG AdjustSize(ULONG nSize)  
    {  
        //定义28个素数（大概是2倍关系增长），用来做hash table的大小  
        const ULONG size_list[] = {  
            53,         97,             193,        389,        769,  
            1543,       3079,           6151,       12289,      24593,  
            49157,      98317,          196613,     393241,     786443,  
            1572869,    3145739,        6291469,    12582917,   25165842,  
            50331553,   100663319,      201326611,  402653189,  805306457,  
            1610612741, 3221225473ul,   4294967291ul  
        };  
        int nlistsize = sizeof(size_list) / sizeof(ULONG);  
        int i = 0;  
        for (;i<nlistsize;i++)  
        {  
            if ( size_list[i] >= nSize )  
                break;  
        }  
        if ( i == nlistsize )  
            i--;  
        return size_list[i];  
    }  
      
    ULONG BkdrHashKey(const string key)   
    {
        ULONG hash = 0;  
        for (auto c : key)
        {
            hash = (hash << 5) - hash + c;// seed is 31
        }
        return (hash & 0x7FFFFFFF);  
    }  
      
protected:  
    ULONG m_nHashSize;              //哈希表大小  
    ULONG m_nCount;                 //哈希表中当前元素个数
    LPAssoc* m_pHashTable;          //哈希表头指针
};

void test1()
{
    cout << "\n\n**************test1***************" << std::endl;

    // Insert
    vector<string> testString{"a", "hello", "American", "hello"};
    HashTable source;
    for ( int i = 0; i < testString.size(); i++)
    {
        if (source.Insert(testString[i]))
            cout << "Insert \"" << left << setw(AlignSize) << testString[i] + "\"" << " into hash table successfully." << endl;
        
        else
            cout << "Fail to insert \"" << left << setw(AlignSize) << testString[i] + "\"" << " into hash table." << endl;
    }

    // Print
    source.PrintTable();
    cout << "m_nHashSize = " << source.GetTableSize() << endl;
    cout << "m_nCount = " << source.GetCount() << endl;
}

void test2()
{
    cout << "\n\n**************test2***************" << std::endl;

    // Insert
    string testString;
    HashTable source;
    source.Insert(testString);

    // Print
    source.PrintTable();
    cout << "m_nHashSize = " << source.GetTableSize() << endl;
    cout << "m_nCount = " << source.GetCount() << endl;
}

void test3()
{
    cout << "\n\n**************test3***************" << std::endl;

    // Insert
    string testString1{"hello"};
    string testString2{"guys"};
    string testString3{"well done!"};
    HashTable source;
    for (int i = 0; i<500000; i++)
    {
        source.Insert(testString1);
        source.Insert(testString2);
        source.Insert(testString3);
    }

    // Print
    source.PrintTable();
    cout << "m_nHashSize = " << source.GetTableSize() << endl;
    cout << "m_nCount = " << source.GetCount() << endl;
}

char randStr()
{
    switch((rand()%3))
    {
        case 1:
            return 'A' + rand()%26;break;
        case 2:
            return 'a' + rand()%26;break;
        default:
            return '0' + rand()%10;break;
    }
}

void test4()
{
    cout << "\n\n**************test4***************" << std::endl;

    // Insert
    int stringMaxNumber = 10;
    int stringNumber;
    HashTable source;
    for (int i = 0; i<500; i++)
    {
        string testString;
        stringNumber = rand() % stringMaxNumber;
        while(stringNumber-- >= 0)
            testString += randStr();

        source.Insert(testString);
    }

    // Print
    source.PrintTable();
    cout << "m_nHashSize = " << source.GetTableSize() << endl;
    cout << "m_nCount = " << source.GetCount() << endl;
}

int main(int argc, char *argv[])
{
    test1();
    test2();
    test3();
    test4();
    return 0;
}