#include "x509source.h"
#include "watcher.h"

void workloadapi_x509Source_onX509ContextCallback(
    workloadapi_X509Context *x509cntx, void *args)
{
    workloadapi_X509Source *source = (workloadapi_X509Source *) args;
    workloadapi_X509Source_applyX509Context(source, x509cntx);
}

workloadapi_X509Source *
workloadapi_NewX509Source(workloadapi_X509SourceConfig *config, err_t *err)
{
    if(!config) {
        config = (workloadapi_X509SourceConfig *) malloc(sizeof *config);
        memset(config, 0, sizeof *config);
    }

    /// TODO: add X509SourceOption to set config

    workloadapi_X509Source *source
        = (workloadapi_X509Source *) malloc(sizeof *source);
    source->closed = true;
    mtx_init(&(source->mtx), mtx_plain);
    mtx_init(&(source->closed_mutex), mtx_plain);
    source->svids = NULL;
    source->bundles = NULL;
    source->config = config;
    if(!source->config->picker) {
        source->config->picker = x509svid_SVID_GetDefaultX509SVID;
    }
    if(!source->config->watcher_config.client_options) {
        arrpush(source->config->watcher_config.client_options,
                workloadapi_Client_defaultOptions);
    }
    workloadapi_X509Callback cb
        = { .args = source,
            .func = workloadapi_x509Source_onX509ContextCallback };
    source->watcher
        = workloadapi_newWatcher(source->config->watcher_config, cb, err);
    if((*err)) {
        workloadapi_X509Source_Free(source);
        return NULL;
    }

    return source;
}

// blocks until first SVID update is received
err_t workloadapi_X509Source_Start(workloadapi_X509Source *source)
{
    if(!source) {
        return ERROR1;
    }
    mtx_lock(&(source->closed_mutex));
    source->closed = false;
    mtx_unlock(&(source->closed_mutex));
    err_t err = workloadapi_Watcher_Start(
        source->watcher); // blocks until first update
    return err;
}

err_t workloadapi_X509Source_Close(workloadapi_X509Source *source)
{
    mtx_lock(&(source->closed_mutex));
    source->closed = true;
    mtx_unlock(&(source->closed_mutex));

    return workloadapi_Watcher_Close(source->watcher);
}

x509svid_SVID *
workloadapi_X509Source_GetX509SVID(workloadapi_X509Source *source, err_t *err)
{
    *err = workloadapi_X509Source_checkClosed(source);
    if(!(*err)) {
        mtx_lock(&(source->mtx));
        x509svid_SVID *svid = source->config->picker
                                  ? source->config->picker(source->svids)
                                  : source->svids[0];
        mtx_unlock(&(source->mtx));

        if(svid) {
            return svid;
        }
        // missing SVID
        *err = ERROR1;
        return NULL;
    }

    return NULL;
}

x509bundle_Bundle *workloadapi_X509Source_GetX509BundleForTrustDomain(
    workloadapi_X509Source *source, spiffeid_TrustDomain *td, err_t *err)
{
    *err = workloadapi_X509Source_checkClosed(source);
    if(!(*err)) {
        return x509bundle_Set_GetX509BundleForTrustDomain(source->bundles, *td,
                                                          err);
    }

    return NULL;
}

err_t workloadapi_X509Source_WaitUntilUpdated(workloadapi_X509Source *source)
{
    return workloadapi_Watcher_WaitUntilUpdated(source->watcher);
}

void workloadapi_X509Source_applyX509Context(workloadapi_X509Source *source,
                                             workloadapi_X509Context *ctx)
{
    mtx_lock(&(source->mtx));
    x509bundle_Set_Free(source->bundles);
    for(size_t i = 0, size = arrlenu(source->svids); i < size; ++i) {
        x509svid_SVID_Free(source->svids[i]);
    }
    arrfree(source->svids);
    source->svids = ctx->svids;
    source->bundles = ctx->bundles;
    mtx_unlock(&(source->mtx));
}

err_t workloadapi_X509Source_checkClosed(workloadapi_X509Source *source)
{
    err_t err = NO_ERROR;
    mtx_lock(&(source->closed_mutex));
    if(source->closed) {
        // source is closed
        err = ERROR1;
    }
    mtx_unlock(&(source->closed_mutex));
    return err;
}

void workloadapi_X509Source_Free(workloadapi_X509Source *source)
{
    if(source) {
        mtx_lock(&(source->mtx));
        x509bundle_Set_Free(source->bundles);

        for(size_t i = 0, size = arrlenu(source->svids); i < size; ++i) {
            x509svid_SVID_Free(source->svids[i]);
        }
        arrfree(source->svids);
        if(source->watcher)
            workloadapi_Watcher_Free(source->watcher);

        mtx_unlock(&(source->mtx));
        free(source);
    }
}
