package GuideModel4 {
action def PublishAndShare {
 first start then writeReview;
 action writeReview; then attachPhotos;
 action attachPhotos; then publish;
 action publish; then sendLink;
 action sendLink; then openReview;
 action openReview; then done;
}
}
