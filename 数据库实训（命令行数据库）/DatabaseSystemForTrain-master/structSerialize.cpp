#include "structSerialize.h"

structSerialize::structSerialize()
{
}

sDBs structSerialize::transformObjectToStruct(DBs dbs)
{
	vector<Database> tempDatabasees = dbs.getDatabasees();
	for (auto i = tempDatabasees.begin(); i < tempDatabasees.end(); ++i)
	{
		sDatabase tempsDatabese;
		vector<Table> tempTables = i->getTables();
		for (auto j = tempTables.begin(); j < tempTables.end(); ++j)
		{
			sTable tempsTable;
			vector<Column> tempColumn = j->getColumnArray();
			for (auto n = tempColumn.begin(); n < tempColumn.end(); ++n)
			{
				sColumn tempsColumn;
				tempsColumn.allowNull = n->getAllowNull();
				tempsColumn.columnName = n->getColumnName();
				tempsColumn.columnType = type(n->getColumnType());
				tempsColumn.length = n->getLength();
				tempsColumn.primaryKey = n->getPrimaryKey();
				scolumn.push_back(tempsColumn);
			}
			vector<Row> tempRow = j->getRowArray();
			for (auto m = tempRow.begin(); m < tempRow.end(); ++m)
			{
				sRow tempsRow;
				for (int r = 0; r < tempColumn.size(); ++r)
				{
					tempsRow.data.push_back(m->getCell(r));
				}
				srow.push_back(tempsRow);
			}
			tempsTable.column = scolumn;
			tempsTable.row = srow;
			tempsTable.columnCount = j->getColumnCount();
			tempsTable.rowCount = j->getRowCount();
			tempsTable.tableName = j->getTableName();

			stables.push_back(tempsTable);
		}
		tempsDatabese.name = i->getDatabaseName();
		tempsDatabese.tableCount = i->getTableCount();
		tempsDatabese.tables = stables;

		sdatabasees.push_back(tempsDatabese);
	}

	sdbs.databasees = sdatabasees;
	return sdbs;
}

DBs* structSerialize::transformStructToObject(sDBs sdbs)
{
	vector<Database> tempDatabasees;
	for (auto i = sdbs.databasees.begin(); i < sdbs.databasees.end(); ++i)
	{
		vector<Table> tempTables;
		for (auto j = i->tables.begin(); j < i->tables.end(); ++j)
		{
			vector<Column> tempColumns;
			vector<Row> tempRows;
			for (auto n = j->column.begin(); n < j->column.end(); ++n)
			{
				Column tempColumn = Column(n->columnName, n->columnType, n->length, n->primaryKey, n->allowNull);
				tempColumns.push_back(tempColumn);
			}
			
			for (auto n = j->row.begin(); n < j->row.end(); ++n)
			{
				int index = 0;
				string* tempData = new string[j->columnCount];
				for (auto r = n->data.begin(); r < n->data.end(); ++r)
				{
					if (index < j->columnCount)
					{
						tempData[index] = n->data[index];
						index++;
					}
					
				}
				Row tempRow = Row(tempData);
				tempRows.push_back(tempRow);
			}
			Table tempTable = Table(j->tableName, j->columnCount, j->rowCount, tempColumns, tempRows);
			//tempTable.showTable();
			tempTables.push_back(tempTable);
		}
		Database tempDatabase = Database(i->name, i->tableCount, tempTables);
		//tempDatabase.showDatabase();
		tempDatabasees.push_back(tempDatabase);
	}

	dbs = new DBs(tempDatabasees);
	//dbs->showDBs();
	return dbs;
}
