#!/bin/bash
SRC_DIR="`dirname ${BASH_SOURCE[0]}`/.."
DATA_DIR="${SRC_DIR}/data"
SQL_DIR="${SRC_DIR}/sql"

STOCK_DATA_FILE="${DATA_DIR}/stocks.csv"
SECURITY_DATA_FILE="${SQL_DIR}/security_data.sql"

TICKER_COL="1"
NAME_COL="2"
EXCHANGE_COL="3"
CATEGORY_COL="4"

UNKOWN_CATEGORY='UNKOWN'
STOCK_FILTER='\.(BA|VI|AX|SA|TO|V|SN|SS|SZ|CO|NX|PA|BE|BM|DU|F|HM|HA|MU|SG|DE|HK|BO|NS|JK|TA|MI|MX|AS|NZ|OL|SI|KS|KQ|BC|BI|MF|MC|MA|ST|SW|TWO|TW|L) *$'

get-unique-columns()
{
  COLUMN_INDEX="$1"
  csvtool -t '~' -u '~' col "${COLUMN_INDEX}" ${STOCK_DATA_FILE}|grep -vE '^ *$'|sed 's/"//g'|sort -u
}

get-col()
{
  ROW="$1"
  COL="$2"
  echo "${ROW}"|csvtool col -t '~' ${2} -
}

echo "-- Exchange Insertions" > ${SECURITY_DATA_FILE}
get-unique-columns ${EXCHANGE_COL}|awk "{ printf (\"INSERT INTO Exchange (ExchangeName) VALUES ('%s');\n\", \$0) }" >> ${SECURITY_DATA_FILE}

echo -e "\n\n-- Category Insertions" >> ${SECURITY_DATA_FILE}
echo "INSERT INTO Category (CategoryName) VALUES ('${UNKOWN_CATEGORY}');" >> ${SECURITY_DATA_FILE}
get-unique-columns ${CATEGORY_COL}|awk "{ printf (\"INSERT INTO Category (CategoryName) VALUES ('%s');\n\", \$0) }" >> ${SECURITY_DATA_FILE}

echo -e "\n\n-- Stock Insertions" >> ${SECURITY_DATA_FILE}
IFS=$'\n'
for stock in `get-unique-columns "1-4"`; do
  TICKER=`get-col "$stock" ${TICKER_COL}`
  NAME="`get-col \"$stock\" ${NAME_COL}|tr -d \"'\\\"\"`"
  EXCHANGE=`get-col "$stock" ${EXCHANGE_COL}`
  CATEGORY=`get-col "$stock" ${CATEGORY_COL}`

  [ -n "`echo ${TICKER}|grep -Ei \"${STOCK_FILTER}\"`" ] && continue
  [ -z "${CATEGORY}" ] && CATEGORY="${UNKOWN_CATEGORY}"

  echo "INSERT INTO Stock (Ticker, StockName, ExchangeId, CategoryId) \
SELECT '${TICKER}','${NAME}',Exchange.ExchangeId,Category.CategoryId FROM Exchange,Category \
WHERE Exchange.ExchangeName='${EXCHANGE}' AND Category.CategoryName='${CATEGORY}';" >> ${SECURITY_DATA_FILE}

done
IFS=$' '
