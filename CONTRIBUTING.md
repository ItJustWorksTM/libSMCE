# FIRST OF ALL, THANK YOU!

While this project is mainly maintained by [@AeroStun](https://github.com/AeroStun), external contributions are more
than welcome.

## What to contribute
libSMCE is still in its early life, so bugfixes,  new features, support for more platforms, user and developer documentation,
as well as more tests, are all good ways to contribute to SMCE.

## Who can contribute

Anyone willing to! Just beware that all contributions to this project are under the ItJustWorks™ copyright attribution,
and the Apache 2.0 license.

## How to contribute

### 0. Check issues
Start by looking at the [issues page](https://github.com/ItJustWorksTM/libSMCE/issues), either to get ideas on what to do,
or check whether someone else is already working on your idea.

### 1. Check discussions
Next, look at the [ideas category in the discussions page](https://github.com/ItJustWorksTM/libSMCE/discussions/categories/ideas).
There you can see if someone has already proposed your idea and what feedback they got.
If nobody ever suggested that idea of yours, open a thread, and tell us everything about it!
We will happily give you feedback regarding the usefulness and feasibility of your idea, as well as tips for getting started on it.

### 2. Fork the repo
This is to avoid cluttering the upstream libSMCE repository with user branches.
It also allows you to experiment with different automation services.

#### 2.x Enable GitHub Actions
Would not want to miss a test failure caused by your patch, would you?

### 3. Work on your own feature branches
i.e., do not work on `master` or `devel`, else you might have trouble updating your fork when upstream gets new commits.

### 4. Open a draft [Pull Request](https://github.com/ItJustWorksTM/libSMCE/pulls) against `devel` on upstream
This is so everyone can easily follow your progress, and for you to ask us questions while citing your changes on our CI runs of your PR.

### 5. Request a review from @AeroStun
Once your PR is passing CI and you are satisfied with your patches, requesting a review from the maintainer will get you
another look at your changes, and point out visible issues.  
Expect to be asked along the lines of:
- Reformat code with ClangFormat
- Fix typos
- Rebase on upstream `devel`
- Squash commits
- Deduplicate code/text
- Add tests for your newly added features
- Document said features
- Explain technical choices
- Use more modern techniques

We may ask many more things, so expect anything, yet fear not, for we will guide you if you ask.

### 6. Address the review comments
... and go back to [[5]](#5-request-a-review-from-aerostun), until you get an approval (e.g., "LGTM" (Looks Good To Me)).

### 7. Enjoy the satisfaction of contributing to the world!
And your features too - we are certain that you will make great use of them.


## FAQ

### Q: Can I add dependencies?
A: Generally no, but it depends. Always ask us about that.

### Q: Do you have a wishlist?
A: Not really, but here's one anyway:
- Better test coverage
- Doxygen-generated user & developer documentation
- Clang-cl support (add to Windows CI matrix)
- FreeBSD support (CPack packaging & CI setup)
- More samples (small example projects to demonstrate features)
- Support for more Arduino libraries
