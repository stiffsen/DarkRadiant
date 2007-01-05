#ifndef GRIDITEM_H_
#define GRIDITEM_H_

#include "igrid.h"

class GridItem
{
	// The grid size this item is representing
	GridSize _gridSize;

	IGridManager* _manager;
	
public:
	GridItem() :
		_gridSize(GRID_8),
		_manager(NULL) 
	{}
	
	// Construct this object with a gridsize as argument
	GridItem(GridSize gridSize, IGridManager* manager) :
		_gridSize(gridSize),
		_manager(manager)
	{}
	
	GridItem(const GridItem& other) :
		_gridSize(other._gridSize),
		_manager(other._manager)
	{}
	
	// Returns the gridsize of this item
	GridSize getGridSize() const {
		return _gridSize;
	}
	
	// The callback that triggers the activation of the gridsize contained in this object 
	void activate() {
		if (_manager != NULL) {
			_manager->setGridSize(_gridSize);
		}
	}
	typedef MemberCaller<GridItem, &GridItem::activate> ActivateCaller;
	
}; // class GridItem

#endif /*GRIDITEM_H_*/
