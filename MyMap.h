//
//  MyMap.h
//  CS32 Project 4
//
//  Created by Luca Matsumoto on 3/9/17.
//  Copyright © 2017 Luca Matsumoto. All rights reserved.
//

#ifndef MyMap_h
#define MyMap_h

template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap();
    ~MyMap();
    void clear();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
private:
    class Node
    {
    public:
        Node(KeyType key, ValueType value)
        {
            m_key = key;
            m_value = value;
            m_left = m_right = nullptr;
        }
    private:
        KeyType m_key;
        ValueType m_value;
        Node* m_left;
        Node* m_right;
        void clear()
        {
            if(m_left != nullptr)
            {
                m_left->clear();
                delete m_left;
            }
            if(m_right != nullptr)
            {
                m_right->clear();
                delete m_right;
            }
        }
        int size()
        {
            int count = 1;
            if(m_left != nullptr)
            {
                count += m_left->size();
            }
            if(m_right != nullptr)
            {
                count += m_right->size();
            }
            return count;
        }
        void associate(const KeyType& key, const ValueType& value)
        {
            if(key < m_key)
            {
                if(m_left == nullptr)
                {
                    m_left = new Node(key, value);
                }
                else
                {
                    associate(key, value);
                }
            }
            else if(key > m_key)
            {
                if(m_right == nullptr)
                {
                    m_right = new Node(key, value);
                }
                else
                {
                    associate(key, value);
                }
            }
            else if(key == m_key)
            {
                m_value = value;
            }
        }
        const ValueType* find(const KeyType& key) const
        {
            if(key < m_key)
            {
                if(m_left == nullptr)
                {
                    return nullptr;
                }
                else
                {
                    return find(key);
                }
            }
            else if(key > m_key)
            {
                if(m_right == nullptr)
                {
                    return nullptr;
                }
                else
                {
                    return find(key);
                }
            }
            else if(key == m_key)
            {
                return m_value;
            }
        }
    };
    Node* m_root;
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>:: MyMap()
{
    m_root = nullptr;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>:: ~MyMap()
{
    clear();
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>:: clear()
{
    if(m_root != nullptr)
    {
        m_root->clear();
        delete m_root;
    }
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>:: size() const
{
    if(m_root != nullptr)
    {
        return m_root->size();
    }
    return 0;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>:: associate(const KeyType& key, const ValueType& value)
{
    if(m_root == nullptr)
    {
        m_root = new Node(key, value);
    }
    m_root->associate(key, value);
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>:: find(const KeyType& key) const
{
    if(m_root == nullptr)
    {
        return nullptr;
    }
    return m_root->find(key);
}

#endif /* MyMap_h */
