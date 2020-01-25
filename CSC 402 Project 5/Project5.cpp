/*
File Name: Project5.cpp
Author: Will St. Onge
Course: CSC 402
Date: 1/24/20
*/

#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

struct ItemInfo
{
	string description;
	double unitPrice;
	double shippingWeight;

	ItemInfo(string description = "Item not found", double unitPrice = 0.0, double shippingWeight = 0.0) : description(description), unitPrice(unitPrice), shippingWeight(shippingWeight) { }
};

typedef map<string, ItemInfo> Catalog;
typedef map<string, int> Order;

void readCatalog(Catalog&, const string&);
void printCatalog(const Catalog&); 
ItemInfo getItemData(const Catalog&, const string&);
void addItem(Order&, const Catalog&, const string&, int);
void removeItem(Order&, const string&, int);
void displayOrderSummary(const Order&, const Catalog&);
void displayOrderItems(const Order&, const Catalog&);

int main()
{
	Catalog catalog;
	Order order;

	try
	{
		readCatalog(catalog, "test.cfg");
		cout << "Unit testing failed" << endl;
		return -1;
	}
	catch (runtime_error e) 
	{ 
		cout << "readCatalog runtime_error caught" << endl << endl;
	}

	readCatalog(catalog, "CatalogData.txt");
	printCatalog(catalog);

	cout << endl;

	ItemInfo i1, i2, i3;

	i1 = getItemData(catalog, "4123BLU");
	i2 = getItemData(catalog, "576361B");
	i3 = getItemData(catalog, "test");

	if (i3.description != "Item not found")
	{
		cout << "Unit testing failed" << endl;
		return -2;
	}

	cout << i2.description << " " << i2.shippingWeight << " " << i2.unitPrice << endl;
	cout << i3.description << " " << i3.shippingWeight << " " << i3.unitPrice << endl << endl;

	try
	{
		addItem(order, catalog, "test", 4);
	}
	catch (logic_error e)
	{
		cout << "addItem logic_error caught" << endl << endl;
	}

	addItem(order, catalog, "4123BLU", 4);
	addItem(order, catalog, "93456C", 2);
	addItem(order, catalog, "93456C", 3);
	addItem(order, catalog, "4123GRE", 9);

	displayOrderItems(order, catalog);

	cout << endl;

	removeItem(order, "4123BLU", 1);
	removeItem(order, "93456C", 2);

	displayOrderItems(order, catalog);

	cout << endl;

	try 
	{
		removeItem(order, "test", 1);
		cout << "Unit testing failed" << endl;
		return -3;
	}
	catch (logic_error e)
	{
		cout << "removeItem logic_error caught" << endl << endl;
	}

	removeItem(order, "4123GRE", 9);
	displayOrderItems(order, catalog);

	cout << endl;

	displayOrderSummary(order, catalog);

	cout << endl << "Unit testing successful" << endl;
	return 0;
}

void displayOrderItems(const Order& order, const Catalog& catalog)
{
	cout << left << setw(10) << "SKU" << setw(53) << "Description" << setw(32) << "Quantity" << endl;
	cout << setfill('=') << setw(95) << "=" << endl;
	cout << fixed << setfill(' ');

	for (Order::const_iterator it = order.begin(); it != order.end(); it++)
		for (Catalog::const_iterator it1 = catalog.begin(); it1 != catalog.end(); it1++)
			if (it1->first == it->first)
				cout << left << setw(10) << it->first << setw(53) << it1->second.description << setw(32) << it->second << endl;	
}

void readCatalog(Catalog& catalog, const string& fileName)
{
	ifstream in(fileName);
	string temp, token;
	vector<string> tokens;

	if (!in.is_open())
		throw runtime_error("File does not exist.");

	while (getline(in, temp))
	{
		stringstream ss(temp);
		while (getline(ss, token, ':'))
			tokens.push_back(token);
		catalog.insert({ tokens.at(0), ItemInfo(tokens.at(1), stod(tokens.at(2)), stod(tokens.at(3))) });
		tokens.clear();
	}
}

ItemInfo getItemData(const Catalog& catalog, const string& sku)
{
	return catalog.find(sku) == catalog.end() ? ItemInfo() : catalog.at(sku);
}

void addItem(Order& order, const Catalog& catalog, const string& sku, int quantity)
{
	ItemInfo info = getItemData(catalog, sku);

	if (catalog.find(sku) == catalog.end())
		throw logic_error("SKU not found in catalog");
	if (order.find(sku) != order.end())
		order.at(sku) += quantity;
	else
		order.insert({ sku, quantity });
}

void removeItem(Order& order, const string& sku, int quantity)
{
	if (order.find(sku) == order.end())
		throw logic_error("SKU not found in orders");
	if (order.at(sku) - quantity < 1)
		order.erase(sku);
	else
		order.at(sku) -= quantity;
}

void printCatalog(const Catalog& catalog)
{
	cout << left << setw(10) << "SKU" << setw(53) << "Description" << setw(12) << " Unit Price" << setw(20) << "Shipping Wt. (lbs.)" << endl;
	cout << setfill('=') << setw(95) << "=" << endl;
	cout << fixed << setprecision(2) << setfill(' ');

	for (Catalog::const_iterator it = catalog.begin(); it != catalog.end(); it++)
		cout << setw(10) << it->first << setw(53) << it->second.description << " " << setw(12) << it->second.unitPrice << setw(20) << it->second.shippingWeight << endl;
}

void displayOrderSummary(const Order& order, const Catalog& catalog)
{
	int size = order.size(), total = 0;
	double totalWeight = 0, totalCost = 0;

	for (Order::const_iterator it = order.begin(); it != order.end(); it++)
	{
		total += it->second;
		for (Catalog::const_iterator it1 = catalog.begin(); it1 != catalog.end(); it1++)
		{
			if (it1->first == it->first)
			{
				totalWeight += it1->second.shippingWeight * (double)it->second;
				totalCost += it1->second.unitPrice * (double)it->second;
			}
		}
	}

	cout << left << "Order Summary" << endl;
	cout << setfill('=') << setw(95) << "=" << endl;
	cout << fixed << setprecision(2) << setfill(' ');
	cout << "Distinct item types: " << size << endl;
	cout << "Total number of items: " << total << endl;
	cout << "Total cost: " << totalCost << endl;
	cout << "Total weight: " << totalWeight << endl;
}