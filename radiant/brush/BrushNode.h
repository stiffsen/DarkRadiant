#pragma once

#include "TexDef.h"
#include "ibrush.h"
#include "Brush.h"
#include "selectionlib.h"
#include "FaceInstance.h"
#include "EdgeInstance.h"
#include "VertexInstance.h"
#include "BrushClipPlane.h"
#include "transformlib.h"

class BrushInstanceVisitor {
public:
    virtual ~BrushInstanceVisitor() {}
	virtual void visit(FaceInstance& face) const = 0;
};

class BrushNode :
	public scene::Node,
	public scene::Cloneable,
	public Snappable,
	public IdentityTransform,
	public Translatable,
	public IBrushNode,
	public Selectable,
	public BrushObserver,
	public SelectionTestable,
	public ComponentSelectionTestable,
	public ComponentEditable,
	public ComponentSnappable,
	public PlaneSelectable,
	public LitObject,
	public Transformable
{
	LightList* m_lightList;

	// The actual contained brush (NO reference)
	Brush m_brush;

	// Implements the Selectable interface
	ObservedSelectable _selectable;

	FaceInstances m_faceInstances;

	typedef std::vector<EdgeInstance> EdgeInstances;
	EdgeInstances m_edgeInstances;
	typedef std::vector<brush::VertexInstance> VertexInstances;
	VertexInstances m_vertexInstances;

	mutable RenderableWireframe m_render_wireframe;

    // Renderable array of vertex and edge points
	mutable RenderablePointVector _selectedPoints;

	mutable AABB m_aabb_component;
	mutable RenderablePointVector _faceCentroidPointsCulled;
	mutable bool m_viewChanged; // requires re-evaluation of view-dependent cached data

	BrushClipPlane m_clipPlane;

	ShaderPtr m_state_selpoint;

	// TRUE if any of the FaceInstance's component selection got changed or transformed
	mutable bool _renderableComponentsNeedUpdate;

public:
	// Constructor
	BrushNode();

	// Copy Constructor
	BrushNode(const BrushNode& other);

	virtual ~BrushNode();

	// IBrushNode implementtation
	virtual Brush& getBrush();
	virtual IBrush& getIBrush();

	std::string name() const {
		return "Brush";
	}

	void lightsChanged();

	// Bounded implementation
	virtual const AABB& localAABB() const;

	// Selectable implementation
	virtual bool isSelected() const;
	virtual void setSelected(bool select);
	virtual void invertSelected();

	// SelectionTestable implementation
	virtual void testSelect(Selector& selector, SelectionTest& test);

	// ComponentSelectionTestable
	bool isSelectedComponents() const;
	void setSelectedComponents(bool select, SelectionSystem::EComponentMode mode);
	void testSelectComponents(Selector& selector, SelectionTest& test, SelectionSystem::EComponentMode mode);

	// override scene::Inode::onRemoveFromScene to deselect the child components
	virtual void onInsertIntoScene();
	virtual void onRemoveFromScene();

	// ComponentEditable implementation
	const AABB& getSelectedComponentsBounds() const;

	// The callback for the ObservedSelectable
	void selectedChanged(const Selectable& selectable);

	void selectedChangedComponent(const Selectable& selectable);

	// PlaneSelectable implementation
	void selectPlanes(Selector& selector, SelectionTest& test, const PlaneCallback& selectedPlaneCallback);
	void selectReversedPlanes(Selector& selector, const SelectedPlanes& selectedPlanes);

	// Snappable implementation
	virtual void snapto(float snap);

	// ComponentSnappable implementation
	void snapComponents(float snap);

	// Translatable implementation
	virtual void translate(const Vector3& translation);

	// Allocates a new node on the heap (via copy construction)
	scene::INodePtr clone() const;

	// BrushObserver implementation
	void clear();
	void reserve(std::size_t size);
	void push_back(Face& face);
	void pop_back();
	void erase(std::size_t index);
	void connectivityChanged();
	void edge_clear();
	void edge_push_back(SelectableEdge& edge);
	void vertex_clear();
	void vertex_push_back(SelectableVertex& vertex);
	void DEBUG_verify();

	// LitObject implementation
	bool intersectsLight(const RendererLight& light) const;
	void insertLight(const RendererLight& light);
	void clearLights();

	// Renderable implementation
	void renderComponents(RenderableCollector& collector, const VolumeTest& volume) const;
	void renderSolid(RenderableCollector& collector, const VolumeTest& volume) const;
	void renderWireframe(RenderableCollector& collector, const VolumeTest& volume) const;
	void setRenderSystem(const RenderSystemPtr& renderSystem);

	void viewChanged() const;
	bool isHighlighted() const;

	void evaluateTransform();

	// Update call, issued by the FilterSystem on potential shader visibility changes
	void updateFaceVisibility();

	void setClipPlane(const Plane3& plane);

	const BrushInstanceVisitor& forEachFaceInstance(const BrushInstanceVisitor& visitor);

	void forEachFaceInstance(const std::function<void(FaceInstance&)>& functor);

protected:
	// Gets called by the Transformable implementation whenever
	// scale, rotation or translation is changed.
	void _onTransformationChanged();

	// Called by the Transformable implementation before freezing
	// or when reverting transformations.
	void _applyTransformation();

private:
	void transformComponents(const Matrix4& matrix);

	void renderSolid(RenderableCollector& collector, const VolumeTest& volume, const Matrix4& localToWorld) const;
	void renderWireframe(RenderableCollector& collector, const VolumeTest& volume, const Matrix4& localToWorld) const;

	void update_selected() const;
	void renderSelectedPoints(RenderableCollector& collector,
                              const VolumeTest& volume,
                              const Matrix4& localToWorld) const;

	void renderClipPlane(RenderableCollector& collector, const VolumeTest& volume) const;
	void evaluateViewDependent(const VolumeTest& volume, const Matrix4& localToWorld) const;

}; // class BrushNode
typedef boost::shared_ptr<BrushNode> BrushNodePtr;
