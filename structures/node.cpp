#include "node.h"

using namespace fl;

/// Constructor initializing internal `Node` elements (pixels).
///
/// \param S An array of pixel coordinates
Node::Node(const std::vector< std::pair< int, int > >& S)
        : _S(S), sizeKnown(false), ncountKnown(false) {
    this->setParent(NULL);
}

/// Constructor initializing internal `Node` elements (pixels),
/// as well as the list of children.
///
/// \param S An array of pixel coordinates
/// \param children An array of pointers to children of type `Node`
///   \remark All children in \p children have to be of the same type
Node::Node(const std::vector< std::pair< int, int > >& S,
	       const std::vector< Node* >& children)
		  : _S(S), _children(children), sizeKnown(false), ncountKnown(false) {
    this->setParent(NULL);
    for (int i=0, szi = this->_children.size(); i < szi; ++i){
        this->_children[i]->setParent(this);
    }
}

Node::Node(const Node& other)
        : _S(other._S), _children(other._children), _parent(other._parent), _level(other._level),
        sizeKnown(other.sizeKnown), ncountKnown(other.ncountKnown),
        referenceImg(other.referenceImg), size(other.size), ncount(other.ncount) {
    this->attributes.insert(other.attributes.begin(), other.attributes.end());
}

/// Allows to access an ancestral `Node` removed for \p depth
/// steps from the current `Node`.
///
/// \param depth The number of links between the `Node` and it's
/// returned parent. Optional parameter
///
/// \return A pointer to the parent of this `Node`. The parent
/// will be \p depth steps removed from the original `Node`.
///
/// \note Calling this function without a parameter and using
/// the default \p depth = 1 returns the direct parent of the
/// `Node`
Node* Node::parent(int depth) const {
    if (depth<=1 || this->_parent == NULL)
        return this->_parent;
    else
        return this->_parent->parent(depth-1);
}

/// Allows to access an ancestral `Node` which is the largest one
/// in the hierarchy with the area no larger than \p perc *100
/// percent of the original `Node` area (or a specified reference
/// area).
///
/// \param perc The maximal allowed area increase factor.
///
/// \param oriSize Reference size (before the increase). To compare
/// to the original `Node` size in the hierarchy, omit this
/// parameter.
/// \n
/// _This parameter is optional and not meant to be used for
/// standard functionality of this function._
///
/// \param jumps Internal parameter to keep track of the number
/// of steps between the original `Node` and the considered ancestor.
/// \n
/// _This parameter should be *omitted* when the function is
/// used for its standard functionality._
///
/// \return A pointer to the appropriate parent or ancestral `Node`.
Node *Node::parentBySize(double perc, int oriSize, int jumps){
//    if (oriSize == 0) // old implementation. Should be fine
//        oriSize = this->elementCount();
//
//    if (this->parent() == NULL || (((double)this->parent()->elementCount()) / (double)oriSize) > perc){
//        if (!jumps && this->parent() && (((double)this->parent()->elementCount()) / (double)oriSize) < (1.6*perc)){
//            return this->parent();
//        }
//        return this;
//    }
//
//    return this->parent()->parentBySize(perc, oriSize, jumps+1);
    std::vector <Node *> dummy;
    this->parentBySize(perc, dummy, oriSize, jumps);
    return dummy.back();
}

/// Allows to access an ancestral `Node` which is the largest one
/// in the hierarchy with the area no larger than \p perc *100
/// percent of the original `Node` area (or a specified reference
/// area).
/// Returns the whole path between the original `Node` and its
/// ancestor.
///
/// \param perc The maximal allowed area increase factor.
///
/// \param pathBetween An array which will contain the pointers
/// to the ancestral `Node`s
/// in order from the closest to the farthest one in the hierarchy
/// after the function executes.
/// The last element of the array will be equal to the return value
/// of the the sister function
/// `parentBySize(double perc, int oriSize, int jumps)`
///
/// \param oriSize Reference size (before the increase). To compare
/// to the original `Node` size in the hierarchy, omit this
/// parameter.
/// \n
/// _This parameter is optional and not meant to be used for
/// standard functionality of this function._
///
/// \param jumps Internal parameter to keep track of the number
/// of steps between the original `Node` and the considered ancestor.
/// \n
/// _This parameter *should be omitted* when the function is
/// used for its standard functionality._
void Node::parentBySize(double perc, std::vector<Node *> &pathBetween, int oriSize, int jumps){
    pathBetween.push_back(this);
    if (oriSize == 0)
        oriSize = this->elementCount();

    if (this->parent() == NULL || (((double)this->parent()->elementCount()) / (double)oriSize) > perc){
        if (!jumps && this->parent() && (((double)this->parent()->elementCount()) / (double)oriSize) < (1.6*perc)){
            pathBetween.push_back(this->parent());
        }
        return;
    }

    this->parent()->parentBySize(perc, pathBetween, oriSize, jumps+1);
    return;
}

/// \return true if the `Node` is a root `Node`, false otherwise.
bool Node::isRoot() const {
    return  ((this->_parent == NULL) || (this->_parent == this));
}


/// Assigns a child `Node` to this `Node`. Also checks for constraint.
///
/// \param ch A pointer to the new child `Node`.
///
/// \note The function will also check if the new child
/// `Node` does not violate any constraint about general `Node`
/// structure (e.g. containing the same pixel twice) or
/// specific constraints coming from specific `Node` type.
/// \remark The child \p ch has to be of the same type as the
/// parent `Node`.
///
/// \remark Invalidates the values stored by:
/// - `elementCount()`
/// - `nodeCount()`
void Node::addChild(Node* ch){
    this->_children.push_back(ch);
    ch->setParent(this);
    this->sizeKnown = false;
    this->ncountKnown = false;
    this->checkConstraints();
    return;
}

/// Assign a pixel with coordinates as an element of this
/// `Node`.
///
/// \param px Coordinates to the new pixel element.
/// \note The function will also check if the new child
/// `Node` does not violate any constraint about general `Node`
/// structure (e.g. containing the same pixel twice) or
/// specific constraints coming from specific `Node` type.
///
/// \remark Invalidates the values stored by:
/// - `elementCount()`
void Node::addElement(const std::pair< int, int >& px){
    this->_S.push_back(px);
    this->sizeKnown = false;
    this->checkConstraints();
    return;
}

/// Get all the elements (pixel coordinates) belonging this
/// `Node` (children pixel coordinates included).
///
/// \param px An array to fill with the coordinates of the
/// pixel elements.
void Node::getElements(std::vector< std::pair< int, int > >& px) const {
    for (int i=0, szi = this->_children.size(); i < szi; ++i)
        this->_children[i]->getElements(px);
    px.insert(px.end(), this->_S.begin(), this->_S.end());
    return;
}

/// Get all the elements (pixel coordinates) added to the
/// hierarchy at this specific `Node`
/// (children pixel coordinates not included).
///
/// \return An array filled with the coordinates of the
/// pixel elements.
const std::vector< std::pair< int, int > >& Node::getOwnElements() const{
    return _S;
}

/// \remark Even though most hierarchies operate with levels of integral
/// type (i.e. integers), the \p level can be any scalar (double value).
///
/// \note Depending on the action performed, only the integral part of
/// the level might be considered.
///
/// \param level The new level to be assigned.
void Node::assignLevel(const double & level) {
    this->_level = level;
}

/// \remark Even though most hierarchies operate with levels of integral
/// type (i.e. integers), the \p level can be any scalar (double value).
///
/// \return The level of the `Node`.
const double & Node::level() const {
    return this->_level;
}

/// \remark To be used when the gray level of a `Node` is a scalar
/// value (i.e. hierarchies from single channel images).
///
/// \return The gray level of the `Node`.
const int & Node::grayLevel() const {
    return this->_grayLevel;
}

/// \remark To be used when the gray level of a `Node` is a vector value
/// (i.e. hierarchies from multichannel images).
///
/// \return A vector of gray levels of the `Node`.
const std::vector<int> & Node::hyperGraylevel() const {
    return this->_hgrayLevels;
}

/// Colors the elements (pixels) of the provided image which
/// correspond by coordinates to the current `Node` elements with
/// the appropriate
/// gray level (determined by the gray level of the `Node`) and
/// it's children.
///
/// \param img The image to be colored.
///
/// \remark To be used when the gray level of a `Node` is a scalar
/// value (i.e. hierarchies from single channel images).
///
/// \note Behavior might not be implemented for all scalar types.
void Node::colorMe(cv::Mat& img) const{
    for (int i=0, szi = this->_S.size(); i < szi; ++i){
        if (img.type() == CV_8U)
            img.at<uchar>(this->_S[i].second, this->_S[i].first) = (uchar)this->grayLevel();
        else // if (img.type() == CV_32S)
            img.at<unsigned short>(this->_S[i].second, this->_S[i].first) = this->grayLevel();
    }

    for (int i=0, szi = this->_children.size(); i < szi; ++i)
        this->_children[i]->colorMe(img);

    return;
}

/// Colors the elements (pixels) on all the provided image channels
/// which correspond by coordinates to the current `Node` elements
/// with the appropriate vectorial values (determined by the
/// vectorial gray level of a `Node`) and it's children.
///
/// \param imgs The channels of the multichannel image to be colored.
///
/// \remark To be used when the gray level of a `Node` is a vector
/// value (i.e. hierarchies from multichannel images).
void Node::colorThem(std::vector <cv::Mat> &imgs) const{
    for (int i=0, szi = this->_S.size(); i < szi; ++i){
        for (int j=0; j < imgs.size(); ++j){
            imgs[j].at<uchar>(this->_S[i].second, this->_S[i].first) = (uchar)this->_hgrayLevels[j];
        }
    }

    for (int i=0, szi = this->_children.size(); i < szi; ++i)
        this->_children[i]->colorThem(imgs);
    return;
}

/// Includes the elements (pixels) of all the children as well as
/// all the elements of this `Node`.
///
/// \return Number of elements of the Node.
///
/// \remark The function uses stores the calculation results
/// and only repeats the calculation if certain actions have
/// occurred between the consecutive calls of this function.
/// These functions are:
/// - `addChild()`
/// - `addElement()`
///
/// \remark Otherwise, consecutive calls to this function
/// run in O(1).
int Node::elementCount (void) {
    if (!(this->sizeKnown)){
        this->size = this->_S.size();
        for (int i=0, szi = this->_children.size(); i < szi; ++i){
            this->size += this->_children[i]->elementCount();
        }

        this->sizeKnown = true;
    }
    return this->size;
}

/// \return Number of `Node`s in the subtree.
///
/// \remark The function uses stores the calculation results
/// and only repeats the calculation if certain actions have
/// occurred between the consecutive calls of this function.
/// These functions are:
/// - `addChild()`
/// - `deleteChild()`
///
/// \remark Otherwise, consecutive calls to this function
/// run in O(1).
int Node::nodeCount (void) {
    if (!(this->ncountKnown)){
        this->ncount = 1;
        for (int i=0, szi = this->_children.size(); i < szi; ++i)
            this->ncount += this->_children[i]->nodeCount();
        this->ncountKnown = true;
    }
    return this->ncount;
}

/// The specific attribute is manipulated through the abstract
/// `Attribute` interface. To specify the desired specific attribute,
/// the static identifier `SpecificAttribute::name` assigned for
/// every attribute should be used.
///
/// \param name The static identifier `name` assigned to the desired
/// specific attribute.
///
/// \return An `Attribute *` to the specific attribute required. Should
/// be cast to the appropriate pointer type for further manipulation.
Attribute* Node::getAttribute(const std::string &name) const{
    std::map<std::string, Attribute *>::const_iterator pos = this->attributes.find(name);
    if (pos == this->attributes.end()){
        return NULL;
    }

    return pos->second;
}

/// The specific attribute is manipulated through the abstract
/// `Attribute` interface. To specify the desired specific attribute,
/// the static identifier `SpecificAttribute::name` assigned for
/// every attribute should be used.
///
/// \param name The static identifier `name` assigned to the desired
/// specific attribute.
///
/// \param cat The array to be filled with the children `Attribute`s.
/// Upon return, contains an `Attribute *` for each child to the specific
/// attribute required. They should be cast to the appropriate pointer
/// type for further manipulation.
void Node::getChildrenAttributes(const std::string &name, std::vector <Attribute *> &cat) const{
    cat.clear();
    if (this->getAttribute(name) != NULL){
        for (int i=0, szi = this->_children.size(); i < szi; ++i){
            cat.push_back(this->_children[i]->getAttribute(name));
        }
    }
    return;
}

/// The specific attributes are specified by their static identifier
/// `SpecificAttribute::name` assigned for every attribute.
///
/// \param name The concatenation of static identifiers `name` assigned
/// to the desired specific attribute. Example:
/// - `AreaAttribute::name + RangeAttribute::name`
///
/// \return An `AnyPatternSpectra2D *` to the specific pattern spectrum
/// requested. Should be cast to the appropriate pointer type for further
/// manipulation.
AnyPatternSpectra2D * Node::getPatternSpectra2D(const std::string &name) const{
    std::map<std::string, AnyPatternSpectra2D *>::const_iterator pos = this->patternspectra.find(name);
    if (pos == this->patternspectra.end()){
        return NULL;
    }

    return pos->second;
}

/// The specific attributes are specified by their static identifier
/// `SpecificAttribute::name` assigned for every attribute.
///
/// \param name The concatenation of static identifiers `name` assigned
/// to the desired specific attribute. Example:
/// - `AreaAttribute::name + RangeAttribute::name`
///
/// \param cps The array to be filled with the children `PatternSpectra2D`s.
/// Upon return, contains an `AnyPatternSpectra *` for each child for
/// the specific pattern spectra required. They should be cast to the
/// appropriate pointer type for further manipulation.
void Node::getChildrenPatternSpectra2D(const std::string &name, std::vector <AnyPatternSpectra2D *> &cps) const{
    cps.clear();
    if (this->getPatternSpectra2D(name) != NULL){
        for (int i=0, szi = this->_children.size(); i < szi; ++i)
            cps.push_back(this->_children[i]->getPatternSpectra2D(name));
    }
    return;
}

/// Deletes a child from this node. Does not check for constraints.
/// This function will assign all the elements (pixels) directly belonging
/// to the child `Node` being deleted to the parent `Node`. Also, all children
/// of the child `Node` become the children of the parent `Node`.
///
/// \param childIndex The index of the child to be deleted.
///
/// \remark This function invalidates `Node::ncountKnown` and triggers a
///  recalculation at next call to `nodeCount()`.
void Node::deleteChild(int childIndex){
    Node *toDelete = this->_children[childIndex];

    if (toDelete->_children.empty()){
        if (childIndex < (int)(this->_children.size()-1)){ // not deleting last child
            this->_children[childIndex] = this->_children.back();
        }
        this->_children.pop_back();
    }
    else{ // who I'm deleting has 1+ children
        this->_children[childIndex] = toDelete->_children[0];
        this->_children.insert(this->_children.end(), ++toDelete->_children.begin(), toDelete->_children.end());

        this->_children[childIndex]->_parent = this;
        for (int i = this->_children.size() - toDelete->_children.size() + 1, szi = this->_children.size(); i < szi; ++i)
            this->_children[i]->_parent = this;
    }
    this->_S.insert(this->_S.end(), toDelete->_S.begin(), toDelete->_S.end());


    Node *crt = toDelete;
    do{
        crt = crt->parent();
        if (!crt->ncountKnown)
            break;
        crt->ncountKnown = false;
    }while (!crt->isRoot());

    delete toDelete;
}

/// \param p `Node *` pointer to a new parent `Node`.
void Node::setParent(Node* p){
    this->_parent = p; return;
}

/// \param allNodes The array to be filled with all the `Node *` to
/// all the descendant nodes from the subtree of the current `Node`.
///
/// \note The new `Node *` will be appended to the end of `allNodes`
/// (i.e. the old elements will not be erased in case `allNodes` is
/// not empty.
void Node::getAllDescendants(std::vector <Node *> &allNodes){
    for (int i=0, szi = this->_children.size(); i < szi; ++i){
        allNodes.push_back(this->_children[i]);
        _children[i]->getAllDescendants(allNodes);
    }
    allNodes.push_back(this);
    return;
}

/// The function will take care of assigning multiple
/// copies of `Attribute` to the `Node`. A corresponding
/// `deleteAttribute()` must be called
/// for each call to this function.
///
/// \note This should eventually allow to change the `AttributeSettings`
/// associated with each assigned copy.
///
/// \remark This function is used through
/// `ImageTree::addAttributeToTree()` which ensures the correctness
/// of input parameters.
///
/// \param nat An `Attribute *` to the specific attribute to assign
/// to the `Node`.
///
/// \param name The static identifier `name` assigned to the desired
/// specific attribute. Example: `AreaAttribute::name`
///
/// \return If the `Attribute` was already assigned to the node, returns
/// a pointer to that copy of the `Attribute`. `NULL` otherwise.
///
/// \note Do I lose memory here when I forget the old attribute?
Attribute *Node::addAttribute(Attribute *nat, const std::string &name){
    ++this->attributeCount[name];
    Attribute *retval = this->attributes[name];
    this->attributes[name] = nat;
    return retval;
}

/// The function will take care of multiple assigned
/// copies of `Attribute` to the `Node`. One call to this function
/// must be made
/// for each call to `addAttribute()`
///
/// \note This should eventually take care of the `AttributeSettings`
/// associated with each assigned copy.
///
/// \remark This function is used through
/// `ImageTree::deleteAttributeFromTree()` which ensures the correctness
/// of input parameters.
///
/// \param name The static identifier `name` assigned to the desired
/// specific attribute. Example: `AreaAttribute::name`
///
/// \return If this was the last copy of the `Attribute` assigned to
/// the node, returns
/// a pointer to that copy of the `Attribute`. `NULL` otherwise.
Attribute *Node::deleteAttribute(const std::string &name){
    std::map<std::string, Attribute *>::iterator pos = this->attributes.find(name);
    if (pos == this->attributes.end()){
        return NULL;
    }
    --this->attributeCount[name];
    if (this->attributeCount[name] == 0){
        Attribute *oat = pos->second;
        this->attributes.erase(pos);
        return oat;
    }
    return NULL;
}

/// \param name The static identifier `name` assigned to the desired
/// specific attribute. Example: `AreaAttribute::name`
bool Node::attributeExists(const std::string &name) const{
    return !(this->attributes.find(name) == this->attributes.end());
}

/// The function will only assign a new specific `PatternSpectra2D`
/// to the `Node` if is was not previously assigned to the `Node`.
///
/// \note A corresponding call to `deletePatternSpectra2D()` should
/// be done for every call to this function.
///
/// \remark This function is used through
/// `ImageTree::addPatternSpectra2DToTree()` which ensures the correctness
/// of input parameters.
///
/// \param aps An `AnyPatternSpectra *` to the specific pattern spectrum to assign
/// to the `Node`.
///
/// \param name The concatenation of static identifiers `name` assigned
/// to the desired specific attribute. Example:
/// - `AreaAttribute::name + RangeAttribute::name`
///
/// \return If the specific `PatternSpectra2D` was already assigned to the `Node`
/// returns `false` (and does not perform assignment). Otherwise, returns `true`.
bool Node::addPatternSpectra2D(AnyPatternSpectra2D *aps, const std::string &name){
    if (this->patternspectra[name] == NULL){
        this->patternspectra[name] = aps;
        return true;
    }
    else{
        return false;
    }
}

/// One call to this function must be made
/// for each call to `addPatternSpectra2D()`
///
/// \remark This function is used through
/// `ImageTree::deletePatternSpectra2DFromTree()` which ensures the correctness
/// of input parameters.
///
/// \param name The concatenation of static identifiers `name` assigned
/// to the desired specific attribute. Example:
/// - `AreaAttribute::name + RangeAttribute::name`
///
/// \return If a copy of the specific `PatternSpectra2D` was assigned to
/// the node, returns
/// a `AnyPatternSpectra2D *` to that copy of the pattern spectrum. `NULL` otherwise.
AnyPatternSpectra2D * Node::deletePatternSpectra2D(const std::string &name){
    std::map<std::string, AnyPatternSpectra2D *>::iterator pos = this->patternspectra.find(name);
    if (pos == this->patternspectra.end())
        return NULL;

    AnyPatternSpectra2D *ops = pos->second;
    this->patternspectra.erase(pos);
    return ops;
}

/// \return A pair made of the minimum and maximum level value within
/// the sub-tree of this `Node`.
std::pair<double, double> Node::minMaxLevel(void) const{
    std::pair<double, double> minmaxval = std::make_pair(-1, -1);
    if (minmaxval.first == -1 || this->level() < minmaxval.first)
        minmaxval.first = this->level();
    if (minmaxval.second == -1 || this->level() > minmaxval.second)
        minmaxval.second = this->level();
    for (int i=0, szi = (int)this->_children.size(); i < szi; ++i){
        std::pair<int,int> childminmax = this->_children[i]->minMaxLevel();
        if (minmaxval.first == -1 || childminmax.first < minmaxval.first)
            minmaxval.first = childminmax.first;
        if (minmaxval.second == -1 || childminmax.second > minmaxval.second)
        minmaxval.second = childminmax.second;
    }
    return minmaxval;
}

/// \note All the parameters are optional.
///
/// \param outStream The output stream to which the data should be
/// printed. By default, it equals to `std::cout`.
///
/// \param depth The current depth in the tree. Corresponds to the
/// amount of indentation printed before the information for that
/// `Node`. By default, it equals \(0\) (i.e. first `Node` information
/// not indented).
///
/// \remark Prints the number and a list of elements (pixels) associated
/// directly to each `Node`, as well as the `Node`'s level and gray level.
void Node::printElements(std::ostream &outStream, int depth) const{
    for (int i=0; i < depth; ++i)
        outStream << "\t";
    if (this->_S.empty())
        outStream << "empty ";
    else{
        outStream << this->_S.size() << " ";
        for (int i=0; i < (int)this->_S.size(); ++i){
            outStream << "(" << this->_S[i].first << " " << this->_S[i].second << ") ";
        }
    }
    outStream << " -> " << this->level() << " " << this->grayLevel() << std::endl;
    for (int i=0, szi = (int)this->_children.size(); i < szi; ++i)
        this->_children[i]->printElements(outStream, depth+1);
    return;
}



/*
bool Node::hasMoreChildrenPS(void) const{
    return (this->chPSCount < (int)this->_children.size());
}

void Node::resetChildrenPSCount(void){
    this->chPSCount = 0;
}

AnyPatternSpectra2D* Node::getNextChildPS(){
    return this->_children[this->chPSCount++]->getMyPatternSpectra2D();
}
*/