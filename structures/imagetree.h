/// \file structures/imagetree.h
/// \author Petra Bosilj

#ifndef IMAGETREE_H
#define IMAGETREE_H

#include <utility>

#include "node.h"

#include "../algorithms/predicate.h"

#include <set>

namespace fl {

class AttributeSettings;
class PatternSpectra2DSettings;

/// \class ImageTree
///
/// \brief A common interface to manipulate all different kinds of tree hierarchies.
///
/// \note The tree is constructed by using the tree construction algorithms from
/// "algorithms/*.h".
class ImageTree {

    public:

        /// \brief The constructor for `ImageTree`
        ImageTree(fl::Node *root, std::pair<int, int> imDim);

        /// \brief Class destructor.
        virtual ~ImageTree();

        /// \brief Flip the settings for `Node` handling on destruct.
        void toggleDelNodesOnDestruct();

        /// \brief Enable calling the destructor for all the `Node`s on destruct.
        void enableDelNodesOnDestruct();

        /// \brief Disable destroying all the `Node`s on destruct.s
        void disableDelNodesOnDestruct();

        /// \brief Sets the image.
        void setImage(const cv::Mat &img);

        /// \brief Checks if the image is set.
        bool imageSet() const;

        /// \brief Unsets the image.
        void unsetImage();

        /// \brief Get the image set for this `ImageTree`.
        const cv::Mat &image() const;

        /// \brief Sets the multichannel image.
        void setImages(const std::vector<cv::Mat> &imgs);

        /// \brief Check if the multichannel image is set.
        bool imagesSet() const;

        /// \brief Unsets the multichannel image.
        void unsetImages();

        /// \brief Get the multichannel image for this `ImageTree`.
        const std::vector<cv::Mat> &images() const;

        /// \brief Get the root of the `ImageTree`.
        const Node *root() const;

        /// \brief Get the width of the image used to construct the `ImageTree`.
        int treeWidth(void) const;

        /// \brief Get the width of the image used to construct the `ImageTree`.
        int treeHeight(void) const;

        /// \brief Get the number of `Node`s in this `ImageTree.
        int countNodes(void) const;

        /// \brief Get a random `Node` from the `ImageTree`.
        Node *randomNode();


        /// \brief Get the minimal and maximal levels present in the `ImageTree`.
        std::pair <double, double> minMaxLevel() const;

        /// \brief Print the whole `ImageTree`.
        void printTree(std::ostream &outStream = std::cout) const;

#if 1
        /// \brief Get the minimal and maximal `Attribute` value present in the `ImageTree`.
        template <typename TAT>
        std::pair<typename TAT::attribute_type, typename TAT::attribute_type> minMaxAttribute() const;

        /// \brief Print the all the `Node`s of the `ImageTree`, and their value of an `Attribute`.
        //where AT is Attribute
        template <typename AT>
        void printTreeWithAttribute(std::ostream &outStream = std::cout) const;

#endif

        /// \brief Displays the image obtained by reconstruction from `ImageTree`.
        void displayTree(const std::string &outPath = "/home/pbosilj/Programming/Trees/filtest.png") const;


//      template<class AT1, class AT2>
//      void printPartialTreeWithSpectrum(std::set <fl::Node *> &toPrint, int cols, int rows);

#if 1
        /// \brief Assign a specific `Attribute` to this `ImageTree`.
        template<class AT> // where AT is Attribute
        void addAttributeToTree(AttributeSettings *settings, bool deleteSettings = true) const;

        /// \brief Delete a specific `Attribute` from this `ImageTree`.
        template<class AT> // where AT is Attribute
        void deleteAttributeFromTree() const;

        /// \brief Check if a specific `Attribute` is assigned to this `ImageTree`.
        template<class AT>
        bool isAttributeInTree() const;

        /// \brief Access the current `AttributeSettings` of a specific `Attribute`
        /// assigned to this `ImageTree`.
        template<class AT>
        AttributeSettings *getAttributeSettings() const;

        /// \brief Changes the settings of a specific `Attribute` assigned to this `ImageTree`.
        template<class AT>
        bool changeAttributeSettings(AttributeSettings *nsettings, bool deleteSettings = true) const;

        /// \brief Ensure that the default settings of a specific `Attribute` are assigned to
        /// this `ImageTree`
        template<class AT>
        void ensureDefaultSettings() const;

        /// \brief Revert from using the default settings for a specific `Attribute`.
        template<class AT>
        void revertSettingsChanges() const;
#endif

        /// \brief Perform a filtering on `ImageTree` by evaluating a
        /// predicate on the values of `Node::level()`.
        template<class Function>
        void filterTreeByLevelPredicate(Function predicate, Node *_root = NULL);

#if 1

        /// \brief Perform a filtering on `ImageTree` by only the most
        /// ancestral `Node` with the same value of `Attribute` along each branch.
        template<class TAT>
        void filterTreeByAttribute(Node *_root = NULL);

        /// \brief Perform a filtering on `ImageTree` by evaluating a predicate
        /// on the values of an `Attribute` for each `Node.
        template<class TAT, class Function>
        void filterTreeByAttributePredicate(Function predicate, Node *_root = NULL);

        /// \brief Assign the values of an `Attribute` as the level to each `Node`.
        template<class TAT>
        void assignAttributeAsLevel(Node *_root = NULL);

#if 2

        /// \brief Filter the `ImageTree` so that reconstruction after the filtering
        /// gives the Attribute Profile for the select increasing `Attribute` for the image.
        template<class TAT, class Function>
        void attributeProfileIncreasing(Function predicate, Node *root = NULL);

        /// \brief Filter the `ImageTree` so that reconstruction after the filtering
        /// gives the Attribute Profile for the select `Attribute` for the image.
        template<class TAT, class Function>
        void attributeProfile(Function predicate, Node *root = NULL);


#endif

#if 3
        /// \brief Assign a `PatternSpectra2D` based on two specific `Attribute`s
        /// to this `ImageTree`.
        template<class AT1, class AT2>
        void addPatternSpectra2DToTree(PatternSpectra2DSettings *settings, bool deleteSettings = true) const;

        /// \brief Delete a `PatternSpectra2D` based on two specific `Attribute`s
        /// from this `ImageTree`.
        template<class AT1, class AT2>
        void deletePatternSpectra2DFromTree() const;

#endif

#endif
        friend void markMserInTree(const ImageTree &tree, int deltaLvl, std::vector <Node *> &mser, std::vector <std::pair <double, int> > &div,
            int maxArea, int minArea, double maxVariation, double minDiversity);


    private:
#if 1
        template<class AT> // where AT is Attribute
        void addAttributeToNode(Node *cur, AttributeSettings *settings) const;

        template<class AT> // where AT is Attribute
        void deleteAttributeFromNode(Node *cur) const;

        template<class AT> // where AT is Attribute
        bool changeAttributeSettingsOfNode(Node *cur, AttributeSettings *nsettings) const;
#if 2
        template<class TAT, class Function>
        std::pair<bool, bool> attributeProfile(Function predicate, Node *root, std::map<Node *, bool> *ires);
#endif
        template<class AT1, class AT2>
        void addPatternSpectra2DToNode(Node *cur, PatternSpectra2DSettings *settings) const;

        template<class AT1, class AT2>
        void deletePatternSpectra2DFromNode(Node *cur) const;
#endif
        fl::Node *_root;
        int width, height;

        /// \brief Calls the destructor for a `Node` in the tree and the
        /// whole sub-hierarchy.
        void deallocateRoot(const Node *n);

        /// \brief Checks the constraints but requires a full tree
        /// traversal. Deprecated. Delete?
        void checkConstraints() const;

        bool randomInit;
        std::vector <fl::Node *> allNodes;

        const cv::Mat *img;
        const std::vector<cv::Mat> *imgs;

        bool delOnDest;
};

}

#include "imagetree.tpp"

#endif // IMAGETREE_H