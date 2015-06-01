//Copyright(C) 2015 xsh525@sina.com
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sub license, and / or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

enum TAG_NAMES
{
    TAG_NAME_ITEM_ID,
    TAG_NAME_ITEM_NAME,
    TAG_NAME_ITEM_QUANTITY,
    TAG_NAME_ITEM_UNIT_PRICE,
    TAG_NAME_COUNT
};

class Goods
{
public:
    Goods(const int index, const wstring& name, const double price);
    const wstring& GetName(void) const { return m_name; };
    const int GetIndex(void) const { return m_id; };
    const double GetUnitPrice(void) const { return m_price; };
    void SetPrice(double price) { m_price = price; };
    shared_ptr<Goods> Clone();

private:
    int     m_id;
    wstring  m_name;
    double  m_price;
};

Goods::Goods(const int index, const wstring& name, const double price)
    :m_id(index), m_name(name), m_price(price)
{}

shared_ptr<Goods> Goods::Clone()
{
    return make_shared<Goods>(m_id, m_name, m_price);
}

typedef struct stock_item
{
    shared_ptr<Goods> spGoods;
    int quantity;
} STOCK_ITEM;

typedef shared_ptr<STOCK_ITEM> SPSTOCK_ITEM;

class StockManager
{
public:
    bool In(const shared_ptr<Goods> goods, const int quantity);
    int  Query(const wstring& name) const;
    int  Query(const int id) const;
    int  Out(const wstring& name, const int quantity);
    int  Out(const int id, const int quantity);
    void Empty(void);
    const vector<SPSTOCK_ITEM>& GetAllItems(void) const;
    SPSTOCK_ITEM GetItem(const int id) const;
    SPSTOCK_ITEM GetItem(const wstring& name) const;
    void Print() const;

private:
    vector<shared_ptr<STOCK_ITEM>> m_stockItems;
};

bool StockManager::In(const shared_ptr<Goods> goods, const int quantity)
{
    auto isExist = false;

    for (auto&& i : m_stockItems)
    {
        if (i->spGoods->GetIndex() == goods->GetIndex())
        {
            isExist = true;
            i->quantity += quantity;
            break;
        }
    }

    if (!isExist)
    {
        auto p = make_shared<STOCK_ITEM>();
        p->spGoods = goods;
        p->quantity = quantity;
        m_stockItems.emplace_back(p);
    }

    return true;
}

void StockManager::Empty()
{
    m_stockItems.clear();
}

int StockManager::Out(const wstring& name, const int quantity)
{
    for (auto&& i : m_stockItems)
    {
        if (i->spGoods->GetName() == name)
        {
            if (quantity <= -1 || i->quantity <= quantity)
            {
                i->quantity = 0;
                return 0;
            }
            else
            {
                i->quantity -= quantity;
                return i->quantity;
            }
        }
    }

    return 0;
}

int StockManager::Out(const int id, const int quantity)
{
    for (auto&& i : m_stockItems)
    {
        if (i->spGoods->GetIndex() == id)
        {
            if (quantity <= -1 || i->quantity <= quantity)
            {
                i->quantity = 0;
                return 0;
            }
            else
            {
                i->quantity -= quantity;
                return i->quantity;
            }
        }
    }

    return 0;
}

const vector<SPSTOCK_ITEM>& StockManager::GetAllItems(void) const
{
    return m_stockItems;
}

SPSTOCK_ITEM StockManager::GetItem(int id) const
{
    for (auto&& i : m_stockItems)
    {
        if (i->spGoods->GetIndex() == id)
        {
            return i;
        }
    }

    return nullptr;
}

SPSTOCK_ITEM StockManager::GetItem(const wstring& name) const
{
    for (auto&& i : m_stockItems)
    {
        if (i->spGoods->GetName() == name)
        {
            return i;
        }
    }

    return nullptr;
}

int  StockManager::Query(const wstring& name) const
{
    for (auto&& i : m_stockItems)
    {
        if (i->spGoods->GetName() == name)
        {
            return i->quantity;
        }
    }

    return 0;
}

int  StockManager::Query(const int id) const
{
    for (auto&& i : m_stockItems)
    {
        if (i->spGoods->GetIndex() == id)
        {
            return i->quantity;
        }
    }

    return 0;
}

void StockManager::Print() const
{
    wstring strTitle[TAG_NAME_COUNT];
    strTitle[TAG_NAME_ITEM_ID] = L"Item ID";
    strTitle[TAG_NAME_ITEM_NAME] = L"Item Name";
    strTitle[TAG_NAME_ITEM_QUANTITY] = L"Quantity";
    strTitle[TAG_NAME_ITEM_UNIT_PRICE] = L"Unit Price";
    for (auto i = 0; i < TAG_NAME_COUNT; ++i)
    {
        wcout.width(16);
        wcout << left << strTitle[i];
    }
    wcout << endl << endl;

    for (auto&& i : m_stockItems)
    {
        wcout.width(16);
        wcout << left << i->spGoods->GetIndex();
        wcout.width(16);
        wcout << i->spGoods->GetName();
        wcout.width(16);
        wcout << i->quantity;
        wcout.width(16);
        wcout.precision(2);
        wcout << fixed << i->spGoods->GetUnitPrice() << endl;
    }
}

class Cashier
{
public:
    Cashier(const shared_ptr<StockManager>& spBasket);
    double CalculateTotalPrice(void) const;
    double CaculateChange(double dCash);
    double GetCash() const;

private:
    shared_ptr<StockManager> m_basket;
    double  m_cash;
};

Cashier::Cashier(const shared_ptr<StockManager>& spBasket)
{
    m_basket = spBasket;
    m_cash = 0.0;
}

double Cashier::CalculateTotalPrice(void) const
{
    double dTotalPrice = 0.0;
    vector<SPSTOCK_ITEM> pvSE = m_basket->GetAllItems();

    for (auto&& i : pvSE)
    {
        dTotalPrice += i->spGoods->GetUnitPrice()*i->quantity;
    }
    return dTotalPrice;
}

double Cashier::CaculateChange(double dCash)
{
    m_cash += dCash;
    return m_cash - CalculateTotalPrice();
}

double Cashier::GetCash() const
{
    return m_cash;
}

void buildStore(const shared_ptr<StockManager>& spStore)
{
    spStore->In(make_shared<Goods>(1, L"Pen", 3.00), 100);
    spStore->In(make_shared<Goods>(2, L"Notebook", 5.20), 80);
    spStore->In(make_shared<Goods>(3, L"TV", 720.00), 10);
    spStore->In(make_shared<Goods>(4, L"Phone", 300.0), 20);
    spStore->In(make_shared<Goods>(5, L"Pear", 3.00), 40);
    spStore->In(make_shared<Goods>(6, L"Beef", 21.00), 20);
    spStore->In(make_shared<Goods>(7, L"Cookie", 5.00), 120);
    spStore->In(make_shared<Goods>(8, L"Cigarette", 3.00), 100);
    spStore->In(make_shared<Goods>(9, L"Wine", 6.00), 60);
}

void printHelp()
{
    wcout << endl << endl;
    wcout.width(35);
    wcout.fill(L'*');
    wcout << right << L"USAGE";
    wcout.width(35);
    wcout << left << L"*" << endl;
    wcout << L"@@@@@@ Input -1 to go to cashier desk @@@@@@\n";
    wcout.width(35);
    wcout.fill(L'*');
    wcout << right << L"USAGE";
    wcout.width(35);
    wcout << left << L"*" << endl;
    wcout.fill(L' ');
    wcout << endl << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
    auto id = 0, quantity = 0;
    auto payment = 0.0;

    auto spStore = make_shared<StockManager>();
    buildStore(spStore);
    auto spBasket = make_shared<StockManager>();
    Cashier check(spBasket);

    wcout << L"------------------------Welcome to my little store! Enjoy your shopping!------------------------" << endl << endl;
    wcout << L"************************Our Provides************************" << endl;
    spStore->Print();
    printHelp();

    do {
        wcout << L"Input Item ID: ";
        if (!(cin >> id))
        {
            wcout << endl << endl << L"Invalid input! Bye bye!" << endl << endl;
            return -1;
        }
        if (-1 == id)
        {
            break;
        }

        SPSTOCK_ITEM spSE = spStore->GetItem(id);
        if (spSE == nullptr)
        {
            wcout << endl << L"Sorry we do not have item: " << id << endl;
            continue;
        }

        wcout << L"How many you want to buy: ";
        if (!(cin >> quantity))
        {
            wcout << endl << endl << "Invalid input! Bye bye!" << endl << endl;
            return -1;
        }

        int iManagerNum = spStore->Query(id);
        if (iManagerNum <= 0) {
            wcout << L"Sorry, " << spSE->spGoods->GetName() << L" have been sold out!" << endl;
        }
        else{
            if (quantity > iManagerNum) {
                wcout << L"Sorry, the " << spSE->spGoods->GetName() << L" only have " << iManagerNum << L" left. Just buy that much." << endl;
                spBasket->In(spSE->spGoods->Clone(), iManagerNum);
                spSE->quantity = 0;
            }
            else{
                wcout << L"Thank You for Your Custom! You have bought: " << endl;
                wcout << L"Item:  " << spSE->spGoods->GetName() << L"   Quantity:    " << quantity << endl << endl;
                spBasket->In(spSE->spGoods->Clone(), quantity);
                spSE->quantity = iManagerNum - quantity;
            }
        }
    } while (true);

    if (spBasket->GetAllItems().size() == 0)
    {
        wcout << endl << endl << L"Bought Nothing" << endl << endl;
        return 0;
    }
    else
    {
        wcout << endl << endl << L"You item list:  " << endl << endl;
        spBasket->Print();
    }

    wcout << endl << endl << L"Please pay the bill " << check.CalculateTotalPrice() << L" bucks" << endl;
    wcout << L"Cash please: ";
    cin >> payment;

    while (check.CaculateChange(payment) < 0)
    {
        wcout << L"Short of " << -check.CaculateChange(0) << L" bucks" << endl;
        wcout << L"Cash please: ";
        cin >> payment;
    }

    wcout << endl << endl << L"Cash received: " << check.GetCash() << L" bucks" << endl;
    wcout << L"Payments:" << check.CalculateTotalPrice() << L" bucks" << endl;
    wcout << L"Changes:" << check.CaculateChange(0) << L" bucks" << endl;
    wcout << L"**********************Welcome Your Next Coming**********************\n";

    return 0;
}

