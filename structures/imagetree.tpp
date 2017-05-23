/// \file structures/imagetree.tpp
/// \author Petra Bosilj

#ifndef TPP_IMAGETREE
#define TPP_IMAGETREE

#include "imagetree.h"

#include <set>

#define markedSelf first
#define markedBranch second

namespace fl{

#if 1

/// \tparam TAT A specific `Attribute` class to be used in the search. Any specific
/// `TypedAttribute` where the `TypedAttribute::value()` returns a scalar types can be used
/// (e.g. `AreaAttribute`).
///
/// \return A pair made of the minimum and maximum level value of the specific
/// `TypedAttribute` among all the `Node`s in this `ImageTree`.
// Where TAT is optional
template <typename TAT>
std::pair<typename TAT::attribute_type, typename TAT::attribute_type> ImageTree::minMaxAttribute(void) const{
    return this->_root->minMaxAttribute<TAT>();
}

/// \tparam AT The specific `TypedAttribute` which is to be printed along the
/// subtree. The `TypedAttribute::value()` must be printable to a stream.
///
/// \note All the parameters are optional.
///
/// \param outStream The output stream to which the data should be
/// printed. By default, it equals to `std::cout`.
///
/// \remark Prints the number and a list of elements (pixels) associated
/// directly to each `Node`, as well as the `Node`'s level and
/// `TypedAttribute::value()` associated to each node.
//where AT is Attribute
template<class AT>
void ImageTree::printTreeWithAttribute(std::ostream &outStream) const {
    this->_root->printElementsWithAttribute<AT>();
}

/// Assigns a specific `TypedAttribute` to all the `Node`s in this `ImageTree`.
/// This ensures reserving the memory for any values that need to be stored for
/// the specific `TypedAttribute` calculation.
///
/// \note A copy of the attribute AT will be created at every `Node`, and
/// `Node::addAttribute()` will be called at each `Node`.
///
/// \note A corresponding call to `deleteAttributeFromTree<AT>()` should be made
/// for every call to this function.
///
/// \tparam AT Specifies the `TypedAttribute` to be assigned (e.g. `AreaAttribute`)
///
/// \param settings A pointer to the settings to be used throughout the hierarchy. The
/// same settings will be assigned to the `Attribute` at every `Node`.
/// The settings corresponding to the right attribute should be used (e.g. when using
/// `AreaAttribute`, `AreaSettings *` should be used).
///
/// \param deleteSettings (optional, default = `true`) Determines whether the \p settings
/// will be deleted after this call to `addAttributeToTree()` or not (allows the creation
/// with the `new` function and takes care of the `delete` if set to `true`). If the
/// `AttributeSettings *` \p settings wants to be preserved after this call for further
/// outside use, parameter should be set to `false`.
// where AT is Attribute
template<class AT>
void ImageTree::addAttributeToTree(AttributeSettings *settings, bool deleteSettings) const{
    this->addAttributeToNode<AT>(this->_root, settings);
    if (deleteSettings)
        delete settings;
}

// where AT is Attribute
template<class AT>
void ImageTree::addAttributeToNode(Node *cur, AttributeSettings *settings) const{
    {
        // TODO check if memory is still lost. If yes, fix leak.
        AT *nat = new AT(cur, this, settings); // how is memory lost here???
        AT *oat = (AT *)(cur->addAttribute(nat, AT::name));
        if (oat != NULL){
            delete oat;
        }
    }
    for (int i=0, szi = cur->_children.size(); i < szi; ++i){
        this->addAttributeToNode<AT>(cur->_children[i], settings);
    }
}

/// The function will take care of multiple assigned
/// copies of `Attribute` to the `ImageTree`.
///
/// \note One call to this function
/// must be made
/// for each call to `addAttributeToTree<AT>()`.
///
/// \note This should eventually take care of the `AttributeSettings`
/// associated with each assigned copy.
///
/// \remark This function is calls `Node::deleteAttribute()` for
/// every `Node` in the `ImageTree` and ensures the correctness
/// of its input parameters.
///
/// \tparam AT Specifies the `TypedAttribute` to be deleted (e.g. `AreaAttribute`)
//where AT is Attribute
template<class AT>
void ImageTree::deleteAttributeFromTree() const{
    this->deleteAttributeFromNode<AT>(this->_root);
}

//where AT is Attribute
template<class AT>
void ImageTree::deleteAttributeFromNode(Node *cur) const{
    {
        AT *oat = (AT *)(cur->deleteAttribute(AT::name));
        if (oat != NULL)
            delete oat;
    }
    for (int i=0, szi = cur->_children.size(); i < szi; ++i){
        deleteAttributeFromNode<AT>(cur->_children[i]);
    }
}

/// Checks if the specific `TypedAttribute` requested is assigned to the
/// `Node`s of this `ImageTree`.
///
/// \tparam AT Specifies the `TypedAttribute` to be queried (e.g. `AreaAttribute`).
///
/// \return `true` if the `TypedAttribute` `AT` is assigned to the `ImageTree`
/// and it's `Node`s, `false` otherwise.
//where AT is attribute
template<class AT>
bool ImageTree::isAttributeInTree() const{
    return this->_root->attributeExists(AT::name);
}

/// If the specific `TypedAttribute` `AT` is assigned to this `ImageTree`,
/// return the pointer to the `AttributeSettings` used (common settings
/// are always used for all the `Node`s in the `ImageTree`).
///
/// \tparam AT Specifies the `TypedAttribute` to be queried (e.g. `AreaAttribute`).
///
/// \return `NULL` if the `TypedAttribute` `AT` is not assigned to this
/// `ImageTree`. Otherwise, returns `AttributeSettings *` used for `AT`.
/// (The return value should be appropriately cast, e.g. to `AreaSettings *`
/// in case `AreaAttribute` is used for the query.
template<class AT>
AttributeSettings * ImageTree::getAttributeSettings() const{
    if (!(this->isAttributeInTree<AT>())){
        std::cerr << "not in tree " << std::endl;
        return NULL;
    }
    return this->_root->getAttribute(AT::name)->getSettings(); // blah
}

/// If the specific `TypedAttribute` `AT` is assigned to this `ImageTree`,
/// change the `AttributeSettings` used for this `Attribute` in the `ImageTree`
/// (common settings are always used for all the `Node`s in the `ImageTree`).
///
/// \tparam AT Specifies the `TypedAttribute` whose settings are to be changed
/// (e.g. `AreaAttribute`).
///
/// \param nsettings A pointer to the new settings to be used throughout the hierarchy. The
/// same settings will be assigned to the `Attribute` at every `Node`.
/// The settings corresponding to the right attribute should be used (e.g. when using
/// `AreaAttribute`, `AreaSettings *` should be used).
///
/// \param deleteSettings (optional, default = `true`) Determines whether the \p nsettings
/// will be deleted after this call to `changeAttributeSettings()` or not (allows the creation
/// with the `new` function and takes care of the `delete` if set to `true`). If the
/// `AttributeSettings *` \p settings wants to be preserved after this call for further
/// outside use, parameter should be set to `false`.
///
/// \return `false` if the `TypedAttribute` `AT` is not assigned to this
/// `ImageTree`. Otherwise, return `true` upon successfully changing the
/// `AttributeSettings`.
///
/// \note This function does not allow access or storage of the old `AttributeSettings`. In
/// case those want to be obtained before deletion, `getAttributeSettings()` should be used.
template<class AT>
bool ImageTree::changeAttributeSettings(AttributeSettings *nsettings, bool deleteSettings) const{
    bool rValue = this->changeAttributeSettingsOfNode<AT>(this->_root, nsettings);

    if (deleteSettings)
        delete nsettings;

    return rValue;
}

template<class AT>
bool ImageTree::changeAttributeSettingsOfNode(Node *cur, AttributeSettings *nsettings) const{
    if (cur->getAttribute(AT::name)->changeSettings(nsettings)){
        for (int i=0, szi = cur->_children.size(); i < szi; ++i){
            this->changeAttributeSettingsOfNode<AT>(cur->_children[i], nsettings);
        }
        return true;
    }
    return false;
}

/// If the specific `TypedAttribute` `AT` is assigned to this `ImageTree`,
/// ensure that the `AttributeSettings` used for this `Attribute` in the `ImageTree`
/// are the default ones for that `Attribute`.
/// (common settings are always used for all the `Node`s in the `ImageTree`).
///
/// \tparam AT Specifies the `TypedAttribute` whose settings are to be changed
/// (e.g. `AreaAttribute`) to default.
///
/// \note Some attributes do not have a possibility to change the `AttributeSettings`.
/// If an `Attribute` wishes to ensure the correct functionality of this function,
/// it should overwrite the function pair `TypedAttribute::ensureDefaultSettings()`
/// and `TypedAttribute::revertSettingsChanges()`.
/// If an `Attribute` implements this function, the previous settings should be
/// stored and then later restored.
///
/// \remark To revert the changes back to the stored `AttributeSettings`, use the
/// function `ImageTree::revertSettingsChanges()`.
template<class AT>
void ImageTree::ensureDefaultSettings() const{
    (this->_root->getAttribute(AT::name))->ensureDefaultSettings();
}
/// If the specific `TypedAttribute` `AT` is assigned to this `ImageTree` and was
/// fixed to use the default `AttributeSettings`,
/// revert that change of the `AttributeSettings` used for this `Attribute` in the `ImageTree`
/// to their previous value.
/// (common settings are always used for all the `Node`s in the `ImageTree`).
///
/// \tparam AT Specifies the `TypedAttribute` whose settings are to be changed
/// (e.g. `AreaAttribute`) to default.
///
/// \note Some attributes do not have a possibility to change the `AttributeSettings`.
/// If an `Attribute` wishes to ensure the correct functionality of this function,
/// it should overwrite the function pair `TypedAttribute::revertSettingsChanges()`
/// and `TypedAttribute::ensureDefaultSettings()`.
/// If an `Attribute` implements this function, the previous settings should be
/// stored and then later restored.
///
/// \remark Used to revert the changes back to the stored `AttributeSettings`, if they
/// have been changed by the function `ImageTree::ensureDefaultSettings()`.
template <class AT>
void ImageTree::revertSettingsChanges() const{
    (this->_root->getAttribute(AT::name))->revertSettingsChanges();
}

#endif

/// Uses the DIRECT filtering rule.
/// \remark TODO: Implement other filtering rules.
///
/// \param predicate The functor object which operates on the `Node`s.
/// \note cf. the class `Predicate` to see the correct form of this
/// functor.
///
/// \param root (optional) Should be omitted if performing the filtering
/// on the whole `ImageTree`. `Node *` to the subtree which should be
/// filtered. If set to anything other than the root of
/// the tree, it will only filter the subtree.
template<class Function>
void ImageTree::filterTreeByLevelPredicate(Function predicate, Node *root){
    if (root == NULL){
        filterTreeByLevelPredicate(predicate, this->_root);
        return;
    }

    bool deletion;
    std::vector<Node *> &chi = root->_children;
    do{
        deletion = false;
        for (int i=0; i < (int)chi.size(); ++i){
            if (predicate(chi[i]->level(), root->level()) == false){
                root->deleteChild(i);
                deletion = true;
            }
        }
    }while(deletion == true);

    for (int i=0, szi = chi.size(); i < szi; ++i)
        filterTreeByLevelPredicate(predicate, chi[i]);
}

#if 1

/// Uses the DIRECT filtering rule.
/// \remark TODO: Implement other filtering rules.
///
/// \param root (optional) Should be omitted if performing the filtering
/// on the whole `ImageTree`. `Node *` to the subtree which should be
/// filtered. If set to anything other than the root of
/// the tree, it will only filter the subtree.
///
/// \tparam TAT Specifies the `TypedAttribute<X>` whose values are used for
/// filtering (e.g. `AreaAttribute`).
///
/// \remark `typename TAT::attribute_type` (that is, the type `X` when `TypedAttribute<X>`
/// is used) must implement the `==` operator
///
/// \note Implements the first step of complexity-driven simplification from
/// P. Bosilj, S. Lefevre, E. Kijak: "Hierarchical Image Representation Simplification
/// Driven by Region Complexity" (for the second step use `assignAttributeAsLevel()`)
///
/// \note Is used when constructing an (omega)-tree from an alpha-tree
template<class TAT>
void ImageTree::filterTreeByAttribute(Node *root){
    filterTreeByAttributePredicate<TAT>(DifferentThanParent<typename TAT::attribute_type>(), root);
}

/// Uses the DIRECT filtering rule.
/// \remark TODO: Implement other filtering rules.
///
/// \tparam TAT Specifies the `TypedAttribute<X>` whose values are used for
/// filtering (e.g. `AreaAttribute`).
///
/// \remark The \param predicate has to take in operands of the type
/// `typename TAT::attribute_type` (that is, the type `X` when `TypedAttribute<X>`
/// is used).
///
/// \tparam predicate The functor object which operates on the `Node`s
/// `TypedAttribute` values.
/// \note cf. the class `Predicate` to see the correct form of this
/// functor.
///
/// \param root (discouraged) Should be omitted if performing the filtering
/// on the whole `ImageTree` (intended usage). `Node *` to the subtree which
/// should be
/// filtered. If set to anything other than the root of
/// the tree, it will only filter the subtree.
template<class TAT, class Function>
void ImageTree::filterTreeByAttributePredicate(Function predicate, Node *root){
    if (root == NULL){
        filterTreeByAttributePredicate<TAT>(predicate, this->_root);
        return;
    }

    bool deletion;
    std::vector<Node *> &chi = root->_children;
    do{
        deletion = false;
        for (int i=0; i < (int)chi.size(); ++i){
            if (predicate(((TAT*)chi[i]->getAttribute(TAT::name))->value(), ((TAT*)root->getAttribute(TAT::name))->value()) == false){
                root->deleteChild(i);
                deletion = true;
            }
        }
        if (deletion && root->getOwnElements().empty() && (!chi.empty() && !chi[0]->getOwnElements().empty())){
            for (int i=0; i < (int)chi.size(); ++i){
                root->deleteChild(i);
            }
        }
    }while(deletion == true);

    for (int i=0, szi = chi.size(); i < szi; ++i)
        filterTreeByAttributePredicate<TAT>(predicate, chi[i]);
}

/// \tparam TAT Specifies the `TypedAttribute<X>` whose values are used as
/// new levels in this `ImageTree` (e.g. `AreaAttribute`).
///
/// \remark `typename TAT::attribute_type` (that is, the type `X` when `TypedAttribute<X>`
/// is used) needs to be a scalar value, i.e. castable to `double`.
///
/// \param root (discouraged) Should be omitted if associating a `TypedAttribute` value
/// to all the `Node`s in the `ImageTree` (intended usage).
/// `Node *` to the subtree which should be
/// filtered. If set to anything other than the root of
/// the tree, it will only filter the subtree.
///
/// \note Implements the second step of complexity-driven simplification from
/// P. Bosilj, S. Lefevre, E. Kijak: "Hierarchical Image Representation Simplification
/// Driven by Region Complexity" (for the first step use `filterTreeByAttribute()`)
template<class TAT>
void ImageTree::assignAttributeAsLevel(Node *root){
    if (root == NULL){
        assignAttributeAsLevel<TAT>(this->_root);
        return;
    }

    root->assignLevel(((TAT*)root->getAttribute(TAT::name))->value());
    std::vector <Node *> &children = root->_children;
    for (int i=0, szi = children.size(); i < szi; ++i)
        assignAttributeAsLevel<TAT>(children[i]);

    return;
}

#if 2

/// \tparam TAT The `TypedAttribute<X>` whose values in the `Node`s of
/// the `ImageTree` are used for attribute profile
/// calculation. `X` must be a scalar value (castable to `double`).
/// TAT must be an increasing `Attribute`.
///
/// \param predicate A functor object representing an increasing
/// predicate to be used for attribute profile calculation.
///
/// \remark The combination of an increasing `Attribute` and `Predicate`
/// used corresponds to the increasing criterion used to construct this
/// attribute profile.
///
/// \param root (discouraged) Should be omitted if performing the filtering
/// on the whole `ImageTree` (intended usage). `Node *` to the subtree which
/// should be
/// filtered. If set to anything other than the root of
/// the tree, it will only filter the subtree.
///
/// \note This function was optimized for increasing criteria, and will work faster
/// than the generic `attributeProfile()` function.
///
/// \remark TODO if all the children are at the same level; they can be deleted
///
/// \remark TODO this function modifies the tree. would be useful to have a copy
/// operator
template<class TAT, class Function>
void ImageTree::attributeProfileIncreasing(Function predicate, Node *root){
    if (root==NULL){
        attributeProfileIncreasing<TAT>(predicate, this->_root);
        return;
    }
    std::vector<Node *> &chi = root->_children;
    for (int i=0, szi = (int)chi.size(); i < szi; ++i){
        if (predicate(((TAT*)chi[i]->getAttribute(TAT::name))->value(), ((TAT*)root->getAttribute(TAT::name))->value()) == false){
            // collapse:
            for (; !(chi[i]->_children.empty());){
                chi[i]->deleteChild(0);
            }
            chi[i]->_grayLevel = root->_grayLevel;
            chi[i]->_hgrayLevels = root->_hgrayLevels;
        }
        else{
            attributeProfileIncreasing<TAT>(predicate, chi[i]);
        }
    }
}

/// \tparam TAT The `TypedAttribute<X>` whose values in the `Node`s of
/// the `ImageTree` are used for attribute profile
/// calculation. `X` must be a scalar value (castable to `double`).
/// TAT can be any kind of `Attribute`.
///
/// \param predicate A functor object representing a
/// predicate to be used for attribute profile calculation.
///
/// \remark Any combination of  `Attribute` and `Predicate` can be
/// used.
///
/// \param root (discouraged) Should be omitted if performing the filtering
/// on the whole `ImageTree` (intended usage). `Node *` to the subtree which
/// should be
/// filtered. If set to anything other than the root of
/// the tree, it will only filter the subtree.
///
/// \note This function should be used for non-increasing criteria. The function
/// `attributeProfileIncreasing()` has been optimized to work with increasing
/// criteria.
///
/// \remark TODO this function modifies the tree. would be useful to have a copy
/// operator.
template<class TAT, class Function>
void ImageTree::attributeProfile(Function predicate, Node *root){
    attributeProfile<TAT>(predicate, root, NULL);
}

template<class TAT, class Function>
std::pair<bool, bool> ImageTree::attributeProfile
        (Function predicate, Node *root, std::map<Node *, bool> *ires){
    std::pair<bool, bool> result = std::make_pair(false, true);
    if (ires == NULL){
        std::map <Node *, bool> *tmp = new std::map <Node *, bool>();
        return attributeProfile<TAT>(predicate, root, tmp);
        delete tmp;
    }
    if (root==NULL){
        return attributeProfile<TAT>(predicate, this->_root, ires);
    }

    std::vector<Node *> &chi = root->_children;
    for (int i=0; i < (int)chi.size(); ++i){
        std::pair<bool,bool> childRes = attributeProfile<TAT>(predicate, chi[i], ires);
        result.markedBranch = result.markedBranch && childRes.markedBranch;
    }

    result.markedBranch = result.markedBranch && !chi.empty();
    result.markedSelf = !result.markedBranch && predicate(((TAT*)root->getAttribute(TAT::name))->value());
    result.markedBranch = result.markedBranch || result.markedSelf;

    (*ires)[root] = result.markedSelf;

    for (int i=0; i < chi.size();){
        // child is good and should be kept:
        if ((*ires)[chi[i]] == true){
            ++i;
            continue;
        }
        // child is bad and a leaf, so we make it like parent:
        if (chi[i]->_children.empty()){
            chi[i]->_grayLevel = root->_grayLevel;
            chi[i]->_hgrayLevels = root->_hgrayLevels;
            ++i;
            continue;
        }
        // child is bad and an inner node; so we assign
        // its subtree and self-pixels to the parent
        root->deleteChild(i);
    }

    if (root == this->_root){
        delete ires;
    }

    return result;
}

#endif

#if 3

/// Assigns a `PatternSpectra2D` specified by two concrete `TypedAttribute`s
/// \p AT1 and \p AT2 to all the `Node`s in this `ImageTree`.
/// This ensures reserving the memory for any values that need to be stored for
/// the `PatternSpectra2D` calculation.
/// The function assumed `TypedAttribute`s \p AT1 and \p AT2 were previously
/// assigned to the tree.
///
/// \note A copy of the attribute `PatternSpectra2D<AT1,AT2>` will be created at
/// every `Node`, and
/// `Node::addPatternSpectra2D()` will be called at each `Node`.
///
/// \note A corresponding call to `deletePatternSpectra2DFromTree<AT1, AT2>()` should be
/// made for every call to this function.
///
/// \tparam AT1 Specifies the first `TypedAttribute` to be used with the
/// `PatternSpectra2D` (e.g. `AreaAttribute`)
///
/// \tparam AT1 Specifies the first `TypedAttribute` to be used with the
/// `PatternSpectra2D` (e.g. `NonCompactnessAttribute`)
///
/// \param settings A pointer to the settings to be used throughout the hierarchy. The
/// same settings will be assigned to the `PatternSpectra2D<AT1,AT2>` at every `Node`.
/// Different `PatternSpectra2DSettings` can be used for every `PatternSpectra2D` based
/// on different `TypedAttribute`s.
///
/// \param deleteSettings (optional, default = `true`) Determines whether the \p settings
/// will be deleted after this call to `addPatternSpectra2DToTree()` or not (allows the creation
/// with the `new` function and takes care of the `delete` if set to `true`). If the
/// `PatternSpectra2DSettings *` \p settings wants to be preserved after this call for further
/// outside use, parameter should be set to `false`.
template<class AT1, class AT2>
void ImageTree::addPatternSpectra2DToTree(PatternSpectra2DSettings *settings, bool deleteSettings) const{
    this->addPatternSpectra2DToNode<AT1, AT2>(this->_root, settings);
    if (deleteSettings)
        delete settings;
}

//namespace fl{
template <typename AT1, typename AT2> class PatternSpectra2D;
//}

template<class AT1, class AT2>
void ImageTree::addPatternSpectra2DToNode(Node *cur, PatternSpectra2DSettings *settings) const{
    {
        PatternSpectra2D<AT1, AT2> *nps = new PatternSpectra2D<AT1, AT2>(cur, this, settings, false);
        if (!(cur->addPatternSpectra2D(nps, AT1::name+AT2::name))){ // false means it was not assigned
            delete nps;
            return; // because if it failed at one `Node` it should at all of them.
        }
    }
    for (int i=0, szi = cur->_children.size(); i < szi; ++i){
        this->addPatternSpectra2DToNode<AT1,AT2>(cur->_children[i], settings);
    }
}

/// Deletes a `PatternSpectra2D` specified by two concrete `TypedAttribute`s
/// \p AT1 and \p AT2 from all the `Node`s in this `ImageTree`.
/// This ensures freeing the memory for any values that need to be stored for
/// the `PatternSpectra2D` calculation.
///
/// \note A call to `Node::deletePatternSpectra2D()` will be made at every
/// `Node`.
///
/// \note A call to this function should be made for every corresponding
/// call to `addPatternSpectra2DToTree<AT1, AT2>()`
///
/// \tparam AT1 Specifies the first `TypedAttribute` defining the
/// `PatternSpectra2D` (e.g. `AreaAttribute`)
///
/// \tparam AT1 Specifies the first `TypedAttribute` defining the
/// `PatternSpectra2D` (e.g. `NonCompactnessAttribute`)
template<class AT1, class AT2>
void ImageTree::deletePatternSpectra2DFromTree() const{
    this->deletePatternSpectra2DFromNode<AT1, AT2>(this->_root);
}

template<class AT1, class AT2>
void ImageTree::deletePatternSpectra2DFromNode(Node *cur) const{
    {
        PatternSpectra2D<AT1, AT2> *ops = (PatternSpectra2D<AT1, AT2> *)(cur->deletePatternSpectra2D(AT1::name+AT2::name));
        if (ops != NULL)
            delete ops;
    }
    for (int i=0, szi = cur->_children.size(); i < szi; ++i){
        deletePatternSpectra2DFromNode<AT1,AT2>(cur->_children[i]);
    }
}

#endif

#endif

}

#endif

//template<class AT1, class AT2>
//void ImageTree::printPartialTreeWithSpectrum(std::set <Node *> &toPrint, int cols, int rows) {
//    this->_root->printPartialWithAttribute<AT1, AT2>(toPrint, cols, rows);
//}

//template<class TAT>
//// where TAT is TypedAttribute<int>
//void ImageTree::filterTreeByTIAttribute(Node *root){
//    if (root == NULL){
//        filterTreeByTIAttribute<TAT>(this->_root);
//        return;
//    }
//
//    int myRange = ((TAT*)root->getAttribute(TAT::name))->value();
//    bool deletion;
//    std::vector<Node *> &chi = root->_children;
//    do{
//        deletion = false;
//        for (int i=0; i < (int)chi.size(); ++i){
//            int childRange = ((TAT*)chi[i]->getAttribute(TAT::name))->value();
//            //if (((TAT*)chi[i]->getAttribute(TAT::name))->value() == ((TAT*)root->getAttribute(TAT::name))->value()){
//            if (childRange == myRange){
//                root->deleteChild(i);
//                deletion = true;
//            }
//        }
//
//    }while(deletion == true);
//
//    for (int i=0, szi = chi.size(); i < szi; ++i)
//        filterTreeByTIAttribute<TAT>(chi[i]);
//}