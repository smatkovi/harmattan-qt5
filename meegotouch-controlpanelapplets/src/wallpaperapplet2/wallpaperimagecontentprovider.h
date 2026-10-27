#ifndef IMAGECONTENTPROVIDER_H
#define IMAGECONTENTPROVIDER_H

#ifdef HAVE_GALLERYCORE
#include <gallerycontentprovider.h>

class ImageContentProviderPrivate;
class GalleryModel;

class ImageContentProvider : public GalleryContentProvider
{
    Q_OBJECT

    public:
        explicit ImageContentProvider (
                GalleryModel   &galleryModel, 
                QObject        *parent = 0);

        virtual ~ImageContentProvider();

    public: 
        QAbstractItemModel *model() const;
        virtual void queryContent(int limit=-1);
        virtual bool isLoadingContent() const;

    private:
        ImageContentProviderPrivate *d_ptr;
        Q_DECLARE_PRIVATE (ImageContentProvider);
        Q_DISABLE_COPY (ImageContentProvider);
};

#endif // HAVE_GALLERYCORE

#endif
