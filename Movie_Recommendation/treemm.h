#ifndef TREEMULTIMAP_INCLUDED
#define TREEMULTIMAP_INCLUDED

#include <vector>

template <typename KeyType, typename ValueType>
class TreeMultimap
{
  public:
    class Iterator
    {
      public:
        Iterator()
        {
            ;
        }

        Iterator(std::vector<ValueType>& values)
        {
            m_valuesPt = &values;
            m_valid = true;
        }

        ValueType& get_value() const
        {
            if (!m_valid)
            {
                ValueType temp;
                return temp;
            }

            return (*m_valuesPt)[m_num];
        }

        bool is_valid() const
        {
            return m_valid;
        }

        void advance()
        {
            if (!m_valid)
                return;

            m_num++;
            if (m_num >= m_valuesPt->size())
                m_valid = false;
        }

      private:
          bool m_valid = false;
          int m_num = 0;
          std::vector<ValueType>* m_valuesPt = nullptr;
    };

    TreeMultimap()
    {
        ;
    }

    ~TreeMultimap()
    {
        deleteNode(m_head);
    }

    void insert(const KeyType& key, const ValueType& value)
    {
        if (m_head == nullptr)
        {
            m_head = new Node;
            m_head->m_key = key;
            m_head->m_values.push_back(value);
            return;
        }

        Node* current = m_head;
        while (true)
        {
            if (key == current->m_key)
            {
                current->m_values.push_back(value);
                return;
            }
            else if (key > current->m_key)
            {
                if (current->m_right == nullptr)
                {
                    current->m_right = new Node;
                    current->m_right->m_key = key;
                    current->m_right->m_values.push_back(value);
                    return;
                }
                current = current->m_right;
            }
            else if (key < current->m_key)
            {
                if (current->m_left == nullptr)
                {
                    current->m_left = new Node;
                    current->m_left->m_key = key;
                    current->m_left->m_values.push_back(value);
                    return;
                }
                current = current->m_left;
            }
        }
    }

    Iterator find(const KeyType& key) const
    {
        Node* current = m_head;
        while (current != nullptr)
        {
            if (key == current->m_key)
            {
                return Iterator(current->m_values);
            }
            else if (key > current->m_key)
                current = current->m_right;
            else
                current = current->m_left;
        }
        Iterator temp;
        return temp;
    }

  private:
      struct Node
      {
          KeyType m_key;
          std::vector<ValueType> m_values;
          Node* m_right = nullptr;
          Node* m_left = nullptr;
      };
      Node* m_head = nullptr;

      void deleteNode(Node* node)
      {
          if (node == nullptr)
              return;
          if (node->m_right != nullptr)
          {
              deleteNode(node->m_right);
          }
          if (node->m_left != nullptr)
          {
              deleteNode(node->m_left);
          }
          delete(node);
      }
};

#endif // TREEMULTIMAP_INCLUDED
