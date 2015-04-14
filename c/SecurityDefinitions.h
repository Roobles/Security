#ifndef SECURITY_DEFINITIONS_H
#define SECURITY_DEFINITIONS_H 1

// Definition file for the security database.

#define TABLE_STOCK "Stock as stk \
JOIN Exchange as exc ON stk.ExchangeId = exc.ExchangeId \
JOIN Category as cat ON stk.CategoryId = cat.CategoryId \
JOIN StockMetaData as mta ON stk.StockId = mta.StockId"

#define TABLE_STOCK_DATA "StockData"

#define F_DATA_STOCK_DATA_ID 0
#define F_DATA_STOCK_DATE 1
#define F_DATA_STOCK_HIGH 2
#define F_DATA_STOCK_LOW 3
#define F_DATA_STOCK_CLOSE 4
#define F_DATA_STOCK_ID 5

#define F_STOCK_ID 0
#define F_STOCK_TICKER 1
#define F_STOCK_NAME 2
#define F_STOCK_EXCHANGE_ID 3
#define F_STOCK_CATEGORY_ID 4
#define F_STOCK_EXCHANGE_NAME 6
#define F_STOCK_CATEGORY_NAME 8
#define F_STOCK_META_DATA_ID 9
#define F_STOCK_FOUNDED 10
#define F_STOCK_IS_ALIVE 11
#define F_STOCK_HAS_DIVIDENDS 12

#endif
